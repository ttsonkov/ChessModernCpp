#pragma once
#include <optional>
#include "Types.hpp"

namespace chess {

/// Represents a chess move from one square to another.
///
/// Special move types:
/// - Castling: King moves two squares toward rook
/// - En-passant: Pawn captures diagonally to empty square
/// - Promotion: Pawn reaches back rank and promotes
struct Move {
    Square from{};
    Square to{};
    std::optional<PieceType> promotion{std::nullopt};
    bool en_passant{false};
    bool castling{false};

    /// Default equality compares only source and destination squares.
    /// Special flags are not considered for move matching.
    [[nodiscard]] constexpr bool operator==(const Move& other) const noexcept {
        return from == other.from && to == other.to;
    }

    /// Check if this is a promotion move.
    [[nodiscard]] constexpr bool isPromotion() const noexcept {
        return promotion.has_value();
    }

    /// Check if this is a special move (castling, en-passant, or promotion).
    [[nodiscard]] constexpr bool isSpecial() const noexcept {
        return castling || en_passant || isPromotion();
    }
};

} // namespace chess
