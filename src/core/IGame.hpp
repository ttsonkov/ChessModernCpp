#pragma once
#include <vector>
#include "Board.hpp"
#include "Move.hpp"

namespace chess {

class IGame {
public:
    virtual ~IGame() = default;
    virtual void newGame() = 0;
    virtual bool makeMove(const Move&) = 0;
    virtual Color sideToMove() const noexcept = 0;
    virtual const Board& board() const noexcept = 0;
    virtual std::vector<Move> legalMoves() const = 0;
};

} // namespace chess
