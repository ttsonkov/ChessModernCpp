#include "Rules.hpp"
#include <optional>
#include <array>
#include <cassert>

namespace chess {

namespace {

constexpr bool inBounds(int r, int f) {
    return r >= 0 && r < Board::size && f >= 0 && f < Board::size;
}

std::optional<Square> findKingSquare(const Board& board, Color side) {
    for (int r = 0; r < Board::size; ++r) {
        for (int f = 0; f < Board::size; ++f) {
            Square s{r, f};
            const auto& opt = board.at(s);
            if (opt.has_value() && opt->type == PieceType::King && opt->color == side) {
                return s;
            }
        }
    }
    return std::nullopt;
}

// Returns true if any piece of attackerColor attacks square 'target' on the given board.
bool isSquareAttacked(const Board& board, Square target, Color attackerColor) {
    // Pawns
    if (attackerColor == Color::White) {
        const std::array<std::pair<int,int>,2> pawnOffsets{{{-1,-1},{-1,1}}};
        for (auto [dr, df] : pawnOffsets) {
            int r = target.rank + dr;
            int f = target.file + df;
            if (!inBounds(r, f)) continue;
            const auto& opt = board.at(Square{r,f});
            if (opt.has_value() && opt->type == PieceType::Pawn && opt->color == attackerColor) return true;
        }
    } else {
        const std::array<std::pair<int,int>,2> pawnOffsets{{{1,-1},{1,1}}};
        for (auto [dr, df] : pawnOffsets) {
            int r = target.rank + dr;
            int f = target.file + df;
            if (!inBounds(r, f)) continue;
            const auto& opt = board.at(Square{r,f});
            if (opt.has_value() && opt->type == PieceType::Pawn && opt->color == attackerColor) return true;
        }
    }

    // Knights
    static constexpr std::array<std::pair<int,int>,8> knightJumps{{
        {-2,-1},{-2,1},{-1,-2},{-1,2},{1,-2},{1,2},{2,-1},{2,1}
    }};
    for (auto [dr, df] : knightJumps) {
        int r = target.rank + dr;
        int f = target.file + df;
        if (!inBounds(r, f)) continue;
        const auto& opt = board.at(Square{r,f});
        if (opt.has_value() && opt->type == PieceType::Knight && opt->color == attackerColor) return true;
    }

    // Sliding pieces and king/queen adjacent checks
    static constexpr std::array<std::pair<int,int>,8> directions{{
        {-1,0},{1,0},{0,-1},{0,1},{-1,-1},{-1,1},{1,-1},{1,1}
    }};
    for (size_t i = 0; i < directions.size(); ++i) {
        int dr = directions[i].first;
        int df = directions[i].second;
        int r = target.rank + dr;
        int f = target.file + df;
        int step = 1;
        while (inBounds(r, f)) {
            const auto& opt = board.at(Square{r,f});
            if (opt.has_value()) {
                if (opt->color == attackerColor) {
                    if (step == 1 && opt->type == PieceType::King) return true;
                    if ((dr == 0 || df == 0) && (opt->type == PieceType::Rook || opt->type == PieceType::Queen)) return true;
                    if ((dr != 0 && df != 0) && (opt->type == PieceType::Bishop || opt->type == PieceType::Queen)) return true;
                }
                break;
            }
            ++step;
            r += dr; f += df;
        }
    }

    return false;
}

} // unnamed

std::vector<Move> Rules::legalMoves(const Board& board, Color side,
                                    const std::optional<Move>& lastMove,
                                    const std::array<bool,4>& castlingRights) const {
    std::vector<Move> legal;

    // Helper lambda to test if a pseudo move leaves king in check.
    auto isMoveLegal = [&](const Move& m) -> bool {
        Board copy = board; // Board supports copy (Grid of optionals)
        // Apply the move to the copy, but for enPassant we must remove the captured pawn.
        copy.movePiece(m);
        if (m.enPassant) {
            // captured pawn is on the same rank as the mover's from, and on the to.file
            Square captured{ m.from.rank, m.to.file };
            copy.at(captured) = std::nullopt;
        }
        // Handle castling doesn't require additional changes for king safety check because the king is moved by movePiece.
        auto kingSq = findKingSquare(copy, side);
        if (!kingSq.has_value()) {
            return false;
        }
        Color opponent = (side == Color::White) ? Color::Black : Color::White;
        return !isSquareAttacked(copy, *kingSq, opponent);
    };

    Color opponent = (side == Color::White) ? Color::Black : Color::White;

    for (int r = 0; r < Board::size; ++r) {
        for (int f = 0; f < Board::size; ++f) {
            Square from{r,f};
            const auto& opt = board.at(from);
            if (!opt.has_value() || opt->color != side) continue;

            const PieceType pt = opt->type;

            if (pt == PieceType::Pawn) {
                int forward = (side == Color::White) ? -1 : 1;
                int startRank = (side == Color::White) ? 6 : 1;
                // Single forward
                Square one{r + forward, f};
                if (inBounds(one.rank, one.file) && !board.at(one).has_value()) {
                    Move m; m.from = from; m.to = one;
                    if (isMoveLegal(m)) legal.push_back(m);

                    // Double forward from starting rank
                    Square two{r + 2*forward, f};
                    if (r == startRank && inBounds(two.rank, two.file) && !board.at(two).has_value()) {
                        Move m2; m2.from = from; m2.to = two;
                        if (isMoveLegal(m2)) legal.push_back(m2);
                    }
                }
                // Captures
                for (int df : {-1, 1}) {
                    Square cap{r + forward, f + df};
                    if (!inBounds(cap.rank, cap.file)) continue;
                    const auto& target = board.at(cap);
                    if (target.has_value() && target->color == opponent) {
                        Move m; m.from = from; m.to = cap;
                        if (isMoveLegal(m)) legal.push_back(m);
                    }
                }

                // En-passant: check lastMove was opponent double pawn move
                if (lastMove.has_value()) {
                    const Move& lm = *lastMove;
                    const auto& movedPieceOpt = board.at(lm.to);
                    // We need to check that last move was a pawn double-step by opponent.
                    // Note: movedPieceOpt refers to the board BEFORE lastMove was made; lm must be used to detect double step.
                    // Detect double-step by rank difference in the last move.
                    if (movedPieceOpt.has_value() && movedPieceOpt->type == PieceType::Pawn &&
                        std::abs(lm.from.rank - lm.to.rank) == 2 && movedPieceOpt->color == opponent) {
                        // square passed over
                        int passedRank = (lm.from.rank + lm.to.rank) / 2;
                        int passedFile = lm.to.file;
                        // Our pawn must be on same rank as lm.to.rank and adjacent file
                        if (r == lm.to.rank && std::abs(f - lm.to.file) == 1) {
                            Square to{passedRank, passedFile};
                            Move m; m.from = from; m.to = to; m.enPassant = true;
                            if (isMoveLegal(m)) legal.push_back(m);
                        }
                    }
                }
            }
            else if (pt == PieceType::Knight) {
                static constexpr std::array<std::pair<int,int>,8> jumps{{
                    {-2,-1},{-2,1},{-1,-2},{-1,2},{1,-2},{1,2},{2,-1},{2,1}
                }};
                for (auto [dr, df] : jumps) {
                    Square to{r + dr, f + df};
                    if (!inBounds(to.rank, to.file)) continue;
                    const auto& dest = board.at(to);
                    if (dest.has_value() && dest->color == side) continue;
                    Move m; m.from = from; m.to = to;
                    if (isMoveLegal(m)) legal.push_back(m);
                }
            }
            else if (pt == PieceType::Bishop || pt == PieceType::Rook || pt == PieceType::Queen) {
                std::vector<std::pair<int,int>> dirs;
                if (pt == PieceType::Bishop || pt == PieceType::Queen) {
                    dirs.push_back({-1,-1}); dirs.push_back({-1,1}); dirs.push_back({1,-1}); dirs.push_back({1,1});
                }
                if (pt == PieceType::Rook || pt == PieceType::Queen) {
                    dirs.push_back({-1,0}); dirs.push_back({1,0}); dirs.push_back({0,-1}); dirs.push_back({0,1});
                }
                for (auto [dr, df] : dirs) {
                    int nr = r + dr;
                    int nf = f + df;
                    while (inBounds(nr, nf)) {
                        Square to{nr, nf};
                        const auto& dest = board.at(to);
                        if (dest.has_value()) {
                            if (dest->color != side) {
                                Move m; m.from = from; m.to = to;
                                if (isMoveLegal(m)) legal.push_back(m);
                            }
                            break; // blocked
                        } else {
                            Move m; m.from = from; m.to = to;
                            if (isMoveLegal(m)) legal.push_back(m);
                        }
                        nr += dr; nf += df;
                    }
                }
            }
            else if (pt == PieceType::King) {
                for (int dr = -1; dr <= 1; ++dr) {
                    for (int df = -1; df <= 1; ++df) {
                        if (dr == 0 && df == 0) continue;
                        Square to{r + dr, f + df};
                        if (!inBounds(to.rank, to.file)) continue;
                        const auto& dest = board.at(to);
                        if (dest.has_value() && dest->color == side) continue;
                        Move m; m.from = from; m.to = to;
                        if (isMoveLegal(m)) legal.push_back(m);
                    }
                }

                // Castling: use castlingRights passed in.
                // Determine king and rook initial squares based on side.
                bool kingSideAllowed = (side == Color::White) ? castlingRights[0] : castlingRights[2];
                bool queenSideAllowed = (side == Color::White) ? castlingRights[1] : castlingRights[3];
                int homeRank = (side == Color::White) ? 7 : 0;
                int kingFile = 4;

                // Only consider castling if king is on its home square and not in check
                if (r == homeRank && f == kingFile && !isSquareAttacked(board, Square{r,f}, opponent)) {
                    // King-side castling (king moves to file 6)
                    if (kingSideAllowed) {
                        // rook must be at file 7 and same color
                        const auto& rookOpt = board.at(Square{homeRank, 7});
                        if (rookOpt.has_value() && rookOpt->type == PieceType::Rook && rookOpt->color == side) {
                            // squares between king and rook must be empty (files 5 and 6)
                            if (!board.at(Square{homeRank,5}).has_value() && !board.at(Square{homeRank,6}).has_value()) {
                                // squares king passes through (5 and 6) must not be attacked
                                if (!isSquareAttacked(board, Square{homeRank,5}, opponent) &&
                                    !isSquareAttacked(board, Square{homeRank,6}, opponent)) {
                                    Move m; m.from = from; m.to = Square{homeRank, 6}; m.castling = true;
                                    if (isMoveLegal(m)) legal.push_back(m);
                                }
                            }
                        }
                    }
                    // Queen-side castling (king moves to file 2)
                    if (queenSideAllowed) {
                        const auto& rookOpt = board.at(Square{homeRank, 0});
                        if (rookOpt.has_value() && rookOpt->type == PieceType::Rook && rookOpt->color == side) {
                            // squares between king and rook must be empty (files 1,2,3)
                            if (!board.at(Square{homeRank,1}).has_value() &&
                                !board.at(Square{homeRank,2}).has_value() &&
                                !board.at(Square{homeRank,3}).has_value()) {
                                // squares king passes through (3 and 2) must not be attacked
                                if (!isSquareAttacked(board, Square{homeRank,3}, opponent) &&
                                    !isSquareAttacked(board, Square{homeRank,2}, opponent)) {
                                    Move m; m.from = from; m.to = Square{homeRank, 2}; m.castling = true;
                                    if (isMoveLegal(m)) legal.push_back(m);
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    return legal;
}

bool Rules::isCheck(const Board& board, Color side) const {
    auto kingSq = findKingSquare(board, side);
    if (!kingSq.has_value()) return false;
    Color opponent = (side == Color::White) ? Color::Black : Color::White;
    return isSquareAttacked(board, *kingSq, opponent);
}

bool Rules::isCheckmate(const Board& board, Color side,
                        const std::optional<Move>& lastMove,
                        const std::array<bool,4>& castlingRights) const {
    if (!isCheck(board, side)) return false;
    auto moves = legalMoves(board, side, lastMove, castlingRights);
    return moves.empty();
}

} // namespace chess
