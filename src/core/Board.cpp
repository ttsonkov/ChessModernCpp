#include "Board.hpp"

namespace chess {

Board::Board() { reset(); }

const std::optional<Piece>& Board::at(Square sq) const noexcept { return board_[sq.rank][sq.file]; }
std::optional<Piece>& Board::at(Square sq) noexcept { return board_[sq.rank][sq.file]; }

void Board::movePiece(const Move& move) {
    board_[move.to.rank][move.to.file] = board_[move.from.rank][move.from.file];
    board_[move.from.rank][move.from.file] = std::nullopt;
}

void Board::reset() {
    // Clear board
    board_ = {};

    // Back-rank order: Rook, Knight, Bishop, Queen, King, Bishop, Knight, Rook
    const PieceType backRank[Board::size] = {
        PieceType::Rook, PieceType::Knight, PieceType::Bishop, PieceType::Queen,
        PieceType::King, PieceType::Bishop, PieceType::Knight, PieceType::Rook
    };

    // Place black pieces on rank 0 and pawns on rank 1
    for (int file = 0; file < Board::size; ++file) {
        board_[0][file] = Piece{backRank[file], Color::Black};
        board_[1][file] = Piece{PieceType::Pawn, Color::Black};
    }

    // Place white pawns on rank 6 and white back-rank on rank 7
    for (int file = 0; file < Board::size; ++file) {
        board_[6][file] = Piece{PieceType::Pawn, Color::White};
        board_[7][file] = Piece{backRank[file], Color::White};
    }
}

} // namespace chess
