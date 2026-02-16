#pragma once
#include <array>
#include <optional>
#include <vector>
#include "Board.hpp"

namespace chess {

/// Chess rules engine for move generation and position evaluation.
/// 
/// Handles all rule-based logic including:
/// - Legal move generation for all piece types
/// - Check, checkmate, and stalemate detection
/// - Special moves (castling, en-passant)
class Rules {
public:
    /// Castling rights array indices.
    /// Order: [WhiteKingside, WhiteQueenside, BlackKingside, BlackQueenside]
    using CastlingRights = std::array<bool, 4>;
    
    /// Generate all legal moves for the given position.
    /// @param board Current board state
    /// @param side Color to move
    /// @param last_move Previous move (for en-passant detection)
    /// @param castling_rights Current castling availability
    /// @return Vector of legal moves
    [[nodiscard]] std::vector<Move> legalMoves(
        const Board& board, 
        Color side,
        const std::optional<Move>& last_move,
        const CastlingRights& castling_rights) const;
    
    /// Check if the given side's king is in check.
    [[nodiscard]] bool isCheck(const Board& board, Color side) const;
    
    /// Check if position is checkmate (king in check with no legal moves).
    [[nodiscard]] bool isCheckmate(
        const Board& board, 
        Color side,
        const std::optional<Move>& last_move,
        const CastlingRights& castling_rights) const;
    
    /// Check if position is stalemate (not in check but no legal moves).
    [[nodiscard]] bool isStalemate(
        const Board& board, 
        Color side,
        const std::optional<Move>& last_move,
        const CastlingRights& castling_rights) const;
};

} // namespace chess
