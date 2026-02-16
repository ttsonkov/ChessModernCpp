#pragma once

/// @file Fwd.hpp
/// @brief Forward declarations for core chess types.
/// Include this header when you only need pointers/references to types.

namespace chess {

// Enums (cannot be forward declared, but listed for documentation)
enum class Color : unsigned char;
enum class PieceType : unsigned char;

// Value types
struct Square;
struct Piece;
struct Move;

// Classes
class Board;
class Rules;
class IGame;
class ChessGame;

} // namespace chess
