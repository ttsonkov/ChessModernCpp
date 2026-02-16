#pragma once
#include "IGame.hpp"
#include "Rules.hpp"
#include <array>
#include <optional>

namespace chess {

/// Concrete implementation of chess game logic.
///
/// Manages the complete game state including:
/// - Board position
/// - Side to move
/// - Castling rights
/// - En-passant eligibility (via last move tracking)
class ChessGame final : public IGame {
public:
    ChessGame();

    // IGame interface
    void newGame() override;
    [[nodiscard]] bool makeMove(const Move& move) override;
    [[nodiscard]] Color sideToMove() const noexcept override;
    [[nodiscard]] const Board& board() const noexcept override;
    [[nodiscard]] std::vector<Move> legalMoves() const override;

    // Extended game status (overrides with implementations)
    [[nodiscard]] bool isCheck() const override;
    [[nodiscard]] bool isGameOver() const override;
    [[nodiscard]] bool isCheckmate() const override;
    [[nodiscard]] bool isStalemate() const override;

private:
    void updateCastlingRights(const Move& move);
    void handleSpecialMoves(const Move& move);

    Board board_;
    Rules rules_;
    Color side_to_move_{Color::White};
    std::optional<Move> last_move_;
    Rules::CastlingRights castling_rights_{true, true, true, true};
};

} // namespace chess
