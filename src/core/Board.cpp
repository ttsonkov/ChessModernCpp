#include "Board.hpp"
#include <algorithm>

namespace chess {

namespace {

/// Standard back-rank piece order: R, N, B, Q, K, B, N, R
constexpr std::array<PieceType, Board::kSize> kBackRankOrder = {
    PieceType::Rook, PieceType::Knight, PieceType::Bishop, PieceType::Queen,
    PieceType::King, PieceType::Bishop, PieceType::Knight, PieceType::Rook
};

} // namespace

Board::Board() { 
    reset(); 
}

const std::optional<Piece>& Board::at(Square sq) const noexcept { 
    return grid_[sq.rank][sq.file]; 
}

std::optional<Piece>& Board::at(Square sq) noexcept { 
    return grid_[sq.rank][sq.file]; 
}

bool Board::hasPieceAt(Square sq) const noexcept {
    return sq.isValid() && grid_[sq.rank][sq.file].has_value();
}

bool Board::hasPieceAt(Square sq, Color color) const noexcept {
    if (!sq.isValid()) return false;
    const auto& piece = grid_[sq.rank][sq.file];
    return piece.has_value() && piece->color == color;
}

void Board::movePiece(const Move& move) {
    grid_[move.to.rank][move.to.file] = grid_[move.from.rank][move.from.file];
    grid_[move.from.rank][move.from.file] = std::nullopt;
}

void Board::setPiece(Square sq, std::optional<Piece> piece) {
    if (sq.isValid()) {
        grid_[sq.rank][sq.file] = piece;
    }
}

void Board::clearSquare(Square sq) {
    setPiece(sq, std::nullopt);
}

void Board::reset() {
    clear();
    setupInitialPosition();
}

void Board::clear() {
    for (auto& row : grid_) {
        std::ranges::fill(row, std::nullopt);
    }
}

std::span<const std::optional<Piece>, Board::kSize> Board::rank(int r) const noexcept {
    return std::span<const std::optional<Piece>, kSize>(grid_[r]);
}

void Board::setupInitialPosition() {
    // Black pieces (ranks 0-1)
    for (int file = 0; file < kSize; ++file) {
        grid_[0][file] = Piece{.type = kBackRankOrder[file], .color = Color::Black};
        grid_[1][file] = Piece{.type = PieceType::Pawn, .color = Color::Black};
    }

    // White pieces (ranks 6-7)
    for (int file = 0; file < kSize; ++file) {
        grid_[6][file] = Piece{.type = PieceType::Pawn, .color = Color::White};
        grid_[7][file] = Piece{.type = kBackRankOrder[file], .color = Color::White};
    }
}

} // namespace chess
