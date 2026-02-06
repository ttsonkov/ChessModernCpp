#pragma once
#include "IInputHandler.hpp"
#include <optional>
#include <SFML/Graphics.hpp>
#include <chrono>

namespace ui {

/// Concrete implementation of input handling for SFML windows.
/// Handles mouse drag-and-drop chess piece movement.
class SfmlInputHandler final : public IInputHandler {
public:
    explicit SfmlInputHandler(sf::RenderWindow& window);
    ~SfmlInputHandler() override = default;

    std::optional<chess::Move> processInput() override;
    void setSelected(std::optional<chess::Square> square) override;
    [[nodiscard]] bool isRunning() const noexcept override;

    // Animation API
    struct AnimationState {
        bool active = false;
        chess::Square from;
        chess::Square to;
        float progress = 0.0f; // 0.0=start, 1.0=end
        std::chrono::steady_clock::time_point startTime;
        float duration = 0.2f; // seconds
    };
    [[nodiscard]] const std::optional<AnimationState>& getAnimation() const noexcept { return animation_; }
    void updateAnimation();
    void startAnimation(chess::Square from, chess::Square to, float duration = 0.2f);
    void stopAnimation();

    /// Get reference to selection for rendering.
    [[nodiscard]] std::optional<chess::Square> getSelected() const noexcept { return selected_; }

    /// Get drag source and position for rendering.
    [[nodiscard]] std::optional<chess::Square> getDragSource() const noexcept { return dragSource_; }
    [[nodiscard]] std::optional<sf::Vector2f> getDragPosition() const noexcept { return dragPosition_; }

    /// Extract visual state for external use.
    /// Allows decoupling input handler from renderer.
    struct VisualState {
        std::optional<chess::Square> selected;
        std::optional<chess::Square> dragSource;
        std::optional<sf::Vector2f> dragPosition;
    };

    [[nodiscard]] VisualState getVisualState() const noexcept {
        return {selected_, dragSource_, dragPosition_};
    }

private:
    sf::RenderWindow& window_;
    bool running_{true};
    std::optional<chess::Square> selected_;
    std::optional<chess::Square> dragSource_;
    std::optional<sf::Vector2f> dragPosition_;
    std::optional<AnimationState> animation_;

    /// Convert window coordinates to board square.
    [[nodiscard]] std::optional<chess::Square> pixelToSquare(int pixelX, int pixelY) const;

    /// Check if clicked square has a valid piece and return it.
    [[nodiscard]] std::optional<chess::Square> getClickedSquare(int pixelX, int pixelY);
};

} // namespace ui
