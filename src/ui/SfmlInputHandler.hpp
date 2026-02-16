#pragma once
#include "IInputHandler.hpp"
#include "Config.hpp"
#include "core/IGame.hpp"
#include <SFML/Graphics.hpp>
#include <chrono>
#include <optional>
#include <span>
#include <vector>

namespace ui {

/// SFML-based input handler for chess piece movement.
///
/// Handles mouse drag-and-drop interaction with smooth animations.
/// Validates moves against the current game state before animating.
class SfmlInputHandler final : public IInputHandler {
public:
    explicit SfmlInputHandler(sf::RenderWindow& window);

    // IInputHandler interface
    [[nodiscard]] std::optional<chess::Move> processInput() override;
    void setSelected(std::optional<chess::Square> square) override;
    [[nodiscard]] bool isRunning() const noexcept override;
    [[nodiscard]] std::optional<chess::Move> updateAnimation() override;
    [[nodiscard]] std::optional<AnimationInfo> getAnimationState() const noexcept override;

    // Animation control
    void startAnimation(chess::Square from, chess::Square to,
                        chess::Piece piece, chess::Move move,
                        float duration = config::kAnimationDuration);
    void stopAnimation();

    // State accessors
    [[nodiscard]] std::optional<chess::Square> getSelected() const noexcept { return selected_; }
    [[nodiscard]] std::optional<chess::Square> getDragSource() const noexcept { return drag_source_; }
    [[nodiscard]] std::optional<sf::Vector2f> getDragPosition() const noexcept { return drag_position_; }
    [[nodiscard]] std::span<const chess::Square> getLegalMoveSquares() const noexcept {
        return legal_move_squares_;
    }

    /// Set the game reference for piece and legal move lookup.
    void setGameRef(const chess::IGame* game) noexcept { game_ref_ = game; }

private:
    /// Internal animation state with timing information.
    struct AnimationState {
        bool active{false};
        chess::Square from{};
        chess::Square to{};
        chess::Piece piece{};
        chess::Move move{};
        float progress{0.0f};
        std::chrono::steady_clock::time_point start_time{};
        float duration{config::kAnimationDuration};
    };

    // Event handlers
    void handleCloseEvent();
    void handleMousePress(int x, int y);
    void handleMouseMove(int x, int y);
    void handleMouseRelease(int x, int y);

    // Coordinate conversion
    [[nodiscard]] std::optional<chess::Square> pixelToSquare(int pixel_x, int pixel_y) const noexcept;

    // Legal move management
    void updateLegalMoves(chess::Square from);
    void clearLegalMoves();
    [[nodiscard]] bool isLegalDestination(chess::Square sq) const noexcept;

    // State
    sf::RenderWindow& window_;
    const chess::IGame* game_ref_{nullptr};
    bool running_{true};

    std::optional<chess::Square> selected_;
    std::optional<chess::Square> drag_source_;
    std::optional<sf::Vector2f> drag_position_;
    std::optional<AnimationState> animation_;
    std::vector<chess::Square> legal_move_squares_;
};

} // namespace ui
