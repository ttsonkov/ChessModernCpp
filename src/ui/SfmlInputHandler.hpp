#pragma once
#include "IInputHandler.hpp"
#include <optional>
#include <SFML/Graphics.hpp>

namespace ui {

/// Concrete implementation of input handling for SFML windows.
/// Handles mouse drag-and-drop chess piece movement.
class SfmlInputHandler : public IInputHandler {
public:
    explicit SfmlInputHandler(sf::RenderWindow& window);
    ~SfmlInputHandler() = default;

    std::optional<chess::Move> processInput() override;
    void setSelected(std::optional<chess::Square> square) override;

    /// Get reference to selection for rendering.
    std::optional<chess::Square> getSelected() const { return selected_; }

    /// Get drag source and position for rendering.
    std::optional<chess::Square> getDragSource() const { return dragSource_; }
    std::optional<sf::Vector2f> getDragPosition() const { return dragPosition_; }

private:
    sf::RenderWindow& window_;
    std::optional<chess::Square> selected_;
    std::optional<chess::Square> dragSource_;
    std::optional<sf::Vector2f> dragPosition_;

    /// Convert window coordinates to board square.
    std::optional<chess::Square> pixelToSquare(int pixelX, int pixelY) const;

    /// Check if clicked square has a valid piece and return it.
    std::optional<chess::Square> getClickedSquare(int pixelX, int pixelY);
};

} // namespace ui
