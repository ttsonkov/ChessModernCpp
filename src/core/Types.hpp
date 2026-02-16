#pragma once
#include <cstdint>
#include <string_view>

namespace chess {

// ============================================================================
// Constants
// ============================================================================

/// Board dimension (8x8 standard chess board).
inline constexpr int kBoardSize = 8;

// ============================================================================
// Enumerations
// ============================================================================

/// Piece color enumeration.
enum class Color : std::uint8_t { 
    White, 
    Black 
};

/// Chess piece type enumeration.
/// Ordered by conventional piece values (Pawn < Knight ? Bishop < Rook < Queen < King).
enum class PieceType : std::uint8_t { 
    Pawn, 
    Knight, 
    Bishop, 
    Rook, 
    Queen, 
    King 
};

// ============================================================================
// Value Types
// ============================================================================

/// Board square coordinates using rank-file notation.
/// @note rank: 0-7 (0 = black back rank/rank 8, 7 = white back rank/rank 1)
/// @note file: 0-7 (0 = a-file, 7 = h-file)
struct Square {
    int rank{0};
    int file{0};
    
    [[nodiscard]] constexpr bool operator==(const Square&) const noexcept = default;
    
    /// Check if this square is within board bounds.
    [[nodiscard]] constexpr bool isValid() const noexcept {
        return rank >= 0 && rank < kBoardSize && file >= 0 && file < kBoardSize;
    }
};

/// Chess piece with type and color.
struct Piece {
    PieceType type{PieceType::Pawn};
    Color color{Color::White};
    
    [[nodiscard]] constexpr bool operator==(const Piece&) const noexcept = default;
};

// ============================================================================
// Utility Functions
// ============================================================================

/// Returns the opposite color.
[[nodiscard]] constexpr Color opponent(Color c) noexcept {
    return (c == Color::White) ? Color::Black : Color::White;
}

/// Returns true if the square coordinates are within board bounds.
[[nodiscard]] constexpr bool isValidSquare(int rank, int file) noexcept {
    return rank >= 0 && rank < kBoardSize && file >= 0 && file < kBoardSize;
}

/// Returns true if the square is within board bounds.
[[nodiscard]] constexpr bool isValidSquare(Square sq) noexcept {
    return sq.isValid();
}

/// Convert color to string representation.
[[nodiscard]] constexpr std::string_view toString(Color c) noexcept {
    return (c == Color::White) ? "White" : "Black";
}

/// Convert piece type to single-character notation.
[[nodiscard]] constexpr char toChar(PieceType type) noexcept {
    constexpr char kPieceChars[] = {'P', 'N', 'B', 'R', 'Q', 'K'};
    const auto idx = static_cast<std::size_t>(type);
    return (idx < sizeof(kPieceChars)) ? kPieceChars[idx] : '?';
}

/// Convert file index (0-7) to algebraic notation (a-h).
[[nodiscard]] constexpr char fileToChar(int file) noexcept {
    return (file >= 0 && file < kBoardSize) ? static_cast<char>('a' + file) : '?';
}

/// Convert rank index (0-7) to algebraic notation (8-1).
[[nodiscard]] constexpr char rankToChar(int rank) noexcept {
    return (rank >= 0 && rank < kBoardSize) ? static_cast<char>('8' - rank) : '?';
}

} // namespace chess
