#pragma once
#include <cstdint>

namespace chess {

enum class Color : std::uint8_t { White, Black };
enum class PieceType : std::uint8_t { Pawn, Knight, Bishop, Rook, Queen, King };

struct Piece final {
    PieceType type;
    Color color;
};

} // namespace chess
