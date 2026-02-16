#pragma once
#include <array>
#include <optional>
#include <span>
#include "Types.hpp"
#include "Move.hpp"

namespace chess {

/// 8x8 chess board representation.
/// 
/// Manages piece placement and basic board operations.
/// Does not enforce rules - that's the responsibility of Rules/ChessGame.
class Board {
public:
    static constexpr int kSize = kBoardSize;
    using Row = std::array<std::optional<Piece>, kSize>;
    using Grid = std::array<Row, kSize>;

    /// Constructs a board with the standard starting position.
    Board();
    
    /// Access piece at square (const).
    /// @pre sq.isValid()
    [[nodiscard]] const std::optional<Piece>& at(Square sq) const noexcept;
    
    /// Access piece at square (mutable).
    /// @pre sq.isValid()
    [[nodiscard]] std::optional<Piece>& at(Square sq) noexcept;
    
    /// Check if a square contains a piece.
    [[nodiscard]] bool hasPieceAt(Square sq) const noexcept;
    
    /// Check if a square contains a piece of the given color.
    [[nodiscard]] bool hasPieceAt(Square sq, Color color) const noexcept;

    /// Move a piece from one square to another.
    /// @pre move.from and move.to are valid squares
    /// @note Does not validate legality - caller must ensure move is legal.
    void movePiece(const Move& move);
    
    /// Place a piece at a square (or clear the square if nullopt).
    void setPiece(Square sq, std::optional<Piece> piece);
    
    /// Clear a square (remove any piece).
    void clearSquare(Square sq);
    
    /// Reset board to standard starting position.
    void reset();
    
    /// Clear all pieces from the board.
    void clear();
    
    /// Get read-only view of a rank (row).
    [[nodiscard]] std::span<const std::optional<Piece>, kSize> rank(int r) const noexcept;

private:
    Grid grid_{};
    
    void setupInitialPosition();
};

} // namespace chess
