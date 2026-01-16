#pragma once
#include "IGame.hpp"
#include "Rules.hpp"
#include <optional>
#include <array>

namespace chess {

class ChessGame : public IGame {
public:
    ChessGame();
    void newGame() override;
    bool makeMove(const Move& move) override;

    Color sideToMove() const noexcept override;
    const Board& board() const noexcept override;
    std::vector<Move> legalMoves() const override;

private:
    Board board_;
    Rules rules_;
    Color side_{Color::White};

    // Keep last move to support en-passant detection
    std::optional<Move> lastMove_;

    // Castling rights: { whiteKingSide, whiteQueenSide, blackKingSide, blackQueenSide }
    std::array<bool,4> castlingRights_{ true, true, true, true };
};

} // namespace chess
