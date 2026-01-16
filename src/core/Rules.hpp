#pragma once
#include <vector>
#include <array>
#include <optional>
#include "Board.hpp"

namespace chess {

class Rules {
public:
    // castlingRights: { whiteKingSide, whiteQueenSide, blackKingSide, blackQueenSide }
    std::vector<Move> legalMoves(const Board& board, Color side,
                                 const std::optional<Move>& lastMove,
                                 const std::array<bool,4>& castlingRights) const;
    bool isCheck(const Board& board, Color side) const;
    bool isCheckmate(const Board& board, Color side,
                     const std::optional<Move>& lastMove,
                     const std::array<bool,4>& castlingRights) const;
};

} // namespace chess
