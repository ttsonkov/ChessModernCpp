#pragma once
#include <optional>
#include "Piece.hpp"

namespace chess {

struct Square { int rank, file; };
constexpr bool operator==(Square a, Square b) { return a.rank == b.rank && a.file == b.file; }

struct Move {
    Square from, to;
    std::optional<PieceType> promotion;
    bool enPassant{false};
    bool castling{false};
};

} // namespace chess
