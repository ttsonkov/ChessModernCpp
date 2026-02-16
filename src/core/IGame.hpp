#pragma once
#include <span>
#include <vector>
#include "Board.hpp"
#include "Move.hpp"

namespace chess {

/// Abstract interface for chess game state and move execution.
///
/// Implementations manage the full game state including:
/// - Board position
/// - Side to move
/// - Castling rights
/// - En-passant eligibility
/// - Move validation
class IGame {
public:
    virtual ~IGame() = default;

    // ========================================================================
    // Game Control
    // ========================================================================

    /// Start a new game with standard starting position.
    virtual void newGame() = 0;

    /// Attempt to make a move.
    /// @param move The move to attempt
    /// @return true if move was legal and applied, false otherwise
    [[nodiscard]] virtual bool makeMove(const Move& move) = 0;

    // ========================================================================
    // State Queries
    // ========================================================================

    /// Get the color of the side to move.
    [[nodiscard]] virtual Color sideToMove() const noexcept = 0;

    /// Get a const reference to the current board state.
    [[nodiscard]] virtual const Board& board() const noexcept = 0;

    /// Get all legal moves for the current position.
    [[nodiscard]] virtual std::vector<Move> legalMoves() const = 0;

    // ========================================================================
    // Game Status (optional overrides with default implementations)
    // ========================================================================

    /// Check if the current side is in check.
    [[nodiscard]] virtual bool isCheck() const { return false; }

    /// Check if the game is over (checkmate or stalemate).
    [[nodiscard]] virtual bool isGameOver() const { return false; }

    /// Check if the current position is checkmate.
    [[nodiscard]] virtual bool isCheckmate() const { return false; }

    /// Check if the current position is stalemate.
    [[nodiscard]] virtual bool isStalemate() const { return false; }
};

} // namespace chess
