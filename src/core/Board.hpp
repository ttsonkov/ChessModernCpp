#pragma once
#include <array>
#include <optional>
#include "Piece.hpp"
#include "Move.hpp"

namespace chess {

class Board {
public:
    static constexpr int size = 8;
    using Grid = std::array<std::array<std::optional<Piece>, size>, size>;

    Board();
    [[nodiscard]] const std::optional<Piece>& at(Square sq) const noexcept;
    [[nodiscard]] std::optional<Piece>& at(Square sq) noexcept;

    void movePiece(const Move& move);
    void reset();

private:
    Grid board_;
};

} // namespace chess
