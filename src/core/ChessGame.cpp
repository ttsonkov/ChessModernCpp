#include "ChessGame.hpp"
#include <algorithm>

namespace chess {

ChessGame::ChessGame() { newGame(); }

void ChessGame::newGame() {
    board_.reset();
    side_ = Color::White;
    lastMove_.reset();
    castlingRights_ = { true, true, true, true };
}

bool ChessGame::makeMove(const Move& move) {
    auto moves = rules_.legalMoves(board_, side_, lastMove_, castlingRights_);
    auto it = std::find_if(moves.begin(), moves.end(),
        [&move](const Move& m){ return m.from == move.from && m.to == move.to; });
    if (it == moves.end()) return false;

    const Move chosen = *it;

    // En-passant: remove the captured pawn which is not on 'to' square
    if (chosen.enPassant) {
        Square captured{ chosen.from.rank, chosen.to.file };
        board_.at(captured) = std::nullopt;
    }

    // Castling: move king then rook; otherwise normal move
    if (chosen.castling) {
        board_.movePiece(chosen);
        int rank = chosen.from.rank;
        if (chosen.to.file == 6) {
            // king-side rook move
            Move rookMove;
            rookMove.from = Square{ rank, 7 };
            rookMove.to = Square{ rank, 5 };
            board_.movePiece(rookMove);
        } else if (chosen.to.file == 2) {
            // queen-side rook move
            Move rookMove;
            rookMove.from = Square{ rank, 0 };
            rookMove.to = Square{ rank, 3 };
            board_.movePiece(rookMove);
        }
    } else {
        board_.movePiece(chosen);
    }

    // Update castling rights:
    // If a king moved, revoke both rights for that color
    if (board_.at(chosen.to).has_value() && board_.at(chosen.to)->type == PieceType::King) {
        if (side_ == Color::White) {
            castlingRights_[0] = false;
            castlingRights_[1] = false;
        } else {
            castlingRights_[2] = false;
            castlingRights_[3] = false;
        }
    }

    // If a rook moved from its initial square, revoke the corresponding right
    if (chosen.from.file == 0 || chosen.from.file == 7) {
        int rank = chosen.from.rank;
        if (rank == 7) {
            if (chosen.from.file == 0) castlingRights_[1] = false; // white queen-side
            if (chosen.from.file == 7) castlingRights_[0] = false; // white king-side
        } else if (rank == 0) {
            if (chosen.from.file == 0) castlingRights_[3] = false; // black queen-side
            if (chosen.from.file == 7) castlingRights_[2] = false; // black king-side
        }
    }

    // If a rook was captured on its initial square, revoke rights for that color
    Square potentialCaptured = chosen.to;
    if (chosen.enPassant) {
        potentialCaptured = Square{ chosen.from.rank, chosen.to.file };
    }
    if (potentialCaptured.rank == 7 && (potentialCaptured.file == 0 || potentialCaptured.file == 7)) {
        if (potentialCaptured.file == 0) castlingRights_[1] = false;
        else castlingRights_[0] = false;
    } else if (potentialCaptured.rank == 0 && (potentialCaptured.file == 0 || potentialCaptured.file == 7)) {
        if (potentialCaptured.file == 0) castlingRights_[3] = false;
        else castlingRights_[2] = false;
    }

    lastMove_ = chosen;
    side_ = (side_ == Color::White) ? Color::Black : Color::White;
    return true;
}

Color ChessGame::sideToMove() const noexcept { return side_; }
const Board& ChessGame::board() const noexcept { return board_; }
std::vector<Move> ChessGame::legalMoves() const { return rules_.legalMoves(board_, side_, lastMove_, castlingRights_); }

} // namespace chess
