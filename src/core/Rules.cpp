#include "Rules.hpp"
#include <array>
#include <cassert>
#include <cmath>
#include <optional>
#include <ranges>
#include <utility>

namespace chess {

namespace {

// Direction vectors for piece movement
constexpr std::array<std::pair<int, int>, 8> kKnightMoves = {{
    {-2, -1}, {-2, 1}, {-1, -2}, {-1, 2},
    {1, -2}, {1, 2}, {2, -1}, {2, 1}
}};

constexpr std::array<std::pair<int, int>, 8> kAllDirections = {{
    {-1, 0}, {1, 0}, {0, -1}, {0, 1},      // Rook directions
    {-1, -1}, {-1, 1}, {1, -1}, {1, 1}     // Bishop directions
}};

constexpr std::array<std::pair<int, int>, 4> kRookDirections = {{
    {-1, 0}, {1, 0}, {0, -1}, {0, 1}
}};

constexpr std::array<std::pair<int, int>, 4> kBishopDirections = {{
    {-1, -1}, {-1, 1}, {1, -1}, {1, 1}
}};

// ============================================================================
// Square Attack Detection
// ============================================================================

[[nodiscard]] std::optional<Square> findKing(const Board& board, Color side) noexcept {
    for (int r = 0; r < Board::kSize; ++r) {
        for (int f = 0; f < Board::kSize; ++f) {
            Square sq{r, f};
            const auto& piece = board.at(sq);
            if (piece && piece->type == PieceType::King && piece->color == side) {
                return sq;
            }
        }
    }
    return std::nullopt;
}

[[nodiscard]] bool isAttackedByPawn(const Board& board, Square target, Color attacker) noexcept {
    const int pawn_direction = (attacker == Color::White) ? -1 : 1;
    const std::array<std::pair<int, int>, 2> offsets = {{{pawn_direction, -1}, {pawn_direction, 1}}};
    
    for (auto [dr, df] : offsets) {
        Square sq{target.rank + dr, target.file + df};
        if (!sq.isValid()) continue;
        
        const auto& piece = board.at(sq);
        if (piece && piece->type == PieceType::Pawn && piece->color == attacker) {
            return true;
        }
    }
    return false;
}

[[nodiscard]] bool isAttackedByKnight(const Board& board, Square target, Color attacker) noexcept {
    for (auto [dr, df] : kKnightMoves) {
        Square sq{target.rank + dr, target.file + df};
        if (!sq.isValid()) continue;
        
        const auto& piece = board.at(sq);
        if (piece && piece->type == PieceType::Knight && piece->color == attacker) {
            return true;
        }
    }
    return false;
}

[[nodiscard]] bool isAttackedBySlider(const Board& board, Square target, Color attacker) noexcept {
    for (std::size_t i = 0; i < kAllDirections.size(); ++i) {
        auto [dr, df] = kAllDirections[i];
        const bool is_diagonal = (i >= 4);
        
        int r = target.rank + dr;
        int f = target.file + df;
        int distance = 1;
        
        while (isValidSquare(r, f)) {
            const auto& piece = board.at(Square{r, f});
            if (piece) {
                if (piece->color == attacker) {
                    // King can attack from distance 1
                    if (distance == 1 && piece->type == PieceType::King) return true;
                    // Rook/Queen for orthogonal, Bishop/Queen for diagonal
                    if (!is_diagonal && (piece->type == PieceType::Rook || piece->type == PieceType::Queen)) return true;
                    if (is_diagonal && (piece->type == PieceType::Bishop || piece->type == PieceType::Queen)) return true;
                }
                break; // Blocked by piece
            }
            ++distance;
            r += dr;
            f += df;
        }
    }
    return false;
}

[[nodiscard]] bool isSquareAttacked(const Board& board, Square target, Color attacker) noexcept {
    return isAttackedByPawn(board, target, attacker) ||
           isAttackedByKnight(board, target, attacker) ||
           isAttackedBySlider(board, target, attacker);
}

// ============================================================================
// Move Generation Helpers
// ============================================================================

[[nodiscard]] bool isMoveLegal(const Board& board, const Move& move, Color side) {
    Board copy = board;
    copy.movePiece(move);
    
    // Handle en-passant capture
    if (move.en_passant) {
        Square captured{move.from.rank, move.to.file};
        copy.clearSquare(captured);
    }
    
    auto king_sq = findKing(copy, side);
    if (!king_sq) return false;
    
    return !isSquareAttacked(copy, *king_sq, opponent(side));
}

void addMoveIfLegal(std::vector<Move>& moves, const Board& board, 
                    Square from, Square to, Color side) {
    Move move{.from = from, .to = to};
    if (isMoveLegal(board, move, side)) {
        moves.push_back(move);
    }
}

void generatePawnMoves(std::vector<Move>& moves, const Board& board, 
                       Square from, Color side, const std::optional<Move>& last_move) {
    const int forward = (side == Color::White) ? -1 : 1;
    const int start_rank = (side == Color::White) ? 6 : 1;
    const Color enemy = opponent(side);
    
    // Single push
    Square one{from.rank + forward, from.file};
    if (one.isValid() && !board.hasPieceAt(one)) {
        addMoveIfLegal(moves, board, from, one, side);
        
        // Double push from starting position
        Square two{from.rank + 2 * forward, from.file};
        if (from.rank == start_rank && two.isValid() && !board.hasPieceAt(two)) {
            addMoveIfLegal(moves, board, from, two, side);
        }
    }
    
    // Captures
    for (int df : {-1, 1}) {
        Square cap{from.rank + forward, from.file + df};
        if (!cap.isValid()) continue;
        
        if (board.hasPieceAt(cap, enemy)) {
            addMoveIfLegal(moves, board, from, cap, side);
        }
    }
    
    // En-passant
    if (last_move) {
        const auto& lm = *last_move;
        if (const auto& moved_piece = board.at(lm.to)) {
            // Check if last move was a double pawn push by opponent
            if (moved_piece->type == PieceType::Pawn && 
                moved_piece->color == enemy &&
                std::abs(lm.from.rank - lm.to.rank) == 2) {
                
                // Check if our pawn is adjacent
                if (from.rank == lm.to.rank && std::abs(from.file - lm.to.file) == 1) {
                    int passed_rank = (lm.from.rank + lm.to.rank) / 2;
                    Move ep_move{
                        .from = from, 
                        .to = Square{passed_rank, lm.to.file},
                        .en_passant = true
                    };
                    if (isMoveLegal(board, ep_move, side)) {
                        moves.push_back(ep_move);
                    }
                }
            }
        }
    }
}

void generateKnightMoves(std::vector<Move>& moves, const Board& board,
                         Square from, Color side) {
    for (auto [dr, df] : kKnightMoves) {
        Square to{from.rank + dr, from.file + df};
        if (!to.isValid()) continue;
        if (board.hasPieceAt(to, side)) continue;
        
        addMoveIfLegal(moves, board, from, to, side);
    }
}

void generateSlidingMoves(std::vector<Move>& moves, const Board& board,
                          Square from, Color side,
                          std::span<const std::pair<int, int>> directions) {
    for (auto [dr, df] : directions) {
        int r = from.rank + dr;
        int f = from.file + df;
        
        while (isValidSquare(r, f)) {
            Square to{r, f};
            const auto& dest = board.at(to);
            
            if (dest) {
                if (dest->color != side) {
                    addMoveIfLegal(moves, board, from, to, side);
                }
                break; // Blocked
            }
            
            addMoveIfLegal(moves, board, from, to, side);
            r += dr;
            f += df;
        }
    }
}

void generateKingMoves(std::vector<Move>& moves, const Board& board,
                       Square from, Color side,
                       const std::array<bool, 4>& castling_rights) {
    const Color enemy = opponent(side);
    
    // Normal king moves
    for (int dr = -1; dr <= 1; ++dr) {
        for (int df = -1; df <= 1; ++df) {
            if (dr == 0 && df == 0) continue;
            
            Square to{from.rank + dr, from.file + df};
            if (!to.isValid()) continue;
            if (board.hasPieceAt(to, side)) continue;
            
            addMoveIfLegal(moves, board, from, to, side);
        }
    }
    
    // Castling
    const int home_rank = (side == Color::White) ? 7 : 0;
    const int king_file = 4;
    
    // Only consider if king is on home square and not in check
    if (from.rank != home_rank || from.file != king_file) return;
    if (isSquareAttacked(board, from, enemy)) return;
    
    const bool kingside = (side == Color::White) ? castling_rights[0] : castling_rights[2];
    const bool queenside = (side == Color::White) ? castling_rights[1] : castling_rights[3];
    
    // Kingside castling
    if (kingside) {
        const auto& rook = board.at(Square{home_rank, 7});
        if (rook && rook->type == PieceType::Rook && rook->color == side) {
            if (!board.hasPieceAt(Square{home_rank, 5}) && 
                !board.hasPieceAt(Square{home_rank, 6})) {
                if (!isSquareAttacked(board, Square{home_rank, 5}, enemy) &&
                    !isSquareAttacked(board, Square{home_rank, 6}, enemy)) {
                    Move castle{
                        .from = from, 
                        .to = Square{home_rank, 6},
                        .castling = true
                    };
                    if (isMoveLegal(board, castle, side)) {
                        moves.push_back(castle);
                    }
                }
            }
        }
    }
    
    // Queenside castling
    if (queenside) {
        const auto& rook = board.at(Square{home_rank, 0});
        if (rook && rook->type == PieceType::Rook && rook->color == side) {
            if (!board.hasPieceAt(Square{home_rank, 1}) &&
                !board.hasPieceAt(Square{home_rank, 2}) &&
                !board.hasPieceAt(Square{home_rank, 3})) {
                if (!isSquareAttacked(board, Square{home_rank, 2}, enemy) &&
                    !isSquareAttacked(board, Square{home_rank, 3}, enemy)) {
                    Move castle{
                        .from = from, 
                        .to = Square{home_rank, 2},
                        .castling = true
                    };
                    if (isMoveLegal(board, castle, side)) {
                        moves.push_back(castle);
                    }
                }
            }
        }
    }
}

} // namespace

// ============================================================================
// Rules Public Interface
// ============================================================================

std::vector<Move> Rules::legalMoves(const Board& board, Color side,
                                    const std::optional<Move>& last_move,
                                    const std::array<bool, 4>& castling_rights) const {
    std::vector<Move> moves;
    moves.reserve(64); // Reasonable initial capacity
    
    for (int r = 0; r < Board::kSize; ++r) {
        for (int f = 0; f < Board::kSize; ++f) {
            Square from{r, f};
            const auto& piece = board.at(from);
            if (!piece || piece->color != side) continue;
            
            switch (piece->type) {
                case PieceType::Pawn:
                    generatePawnMoves(moves, board, from, side, last_move);
                    break;
                    
                case PieceType::Knight:
                    generateKnightMoves(moves, board, from, side);
                    break;
                    
                case PieceType::Bishop:
                    generateSlidingMoves(moves, board, from, side, kBishopDirections);
                    break;
                    
                case PieceType::Rook:
                    generateSlidingMoves(moves, board, from, side, kRookDirections);
                    break;
                    
                case PieceType::Queen:
                    generateSlidingMoves(moves, board, from, side, kAllDirections);
                    break;
                    
                case PieceType::King:
                    generateKingMoves(moves, board, from, side, castling_rights);
                    break;
            }
        }
    }
    
    return moves;
}

bool Rules::isCheck(const Board& board, Color side) const {
    auto king_sq = findKing(board, side);
    return king_sq && isSquareAttacked(board, *king_sq, opponent(side));
}

bool Rules::isCheckmate(const Board& board, Color side,
                        const std::optional<Move>& last_move,
                        const std::array<bool, 4>& castling_rights) const {
    return isCheck(board, side) && legalMoves(board, side, last_move, castling_rights).empty();
}

bool Rules::isStalemate(const Board& board, Color side,
                        const std::optional<Move>& last_move,
                        const std::array<bool, 4>& castling_rights) const {
    return !isCheck(board, side) && legalMoves(board, side, last_move, castling_rights).empty();
}

} // namespace chess
