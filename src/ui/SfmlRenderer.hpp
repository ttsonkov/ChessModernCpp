#pragma once
#include "IRenderer.hpp"
#include <SFML/Graphics.hpp>
#include <optional>
#include <array>
#include <string>
#include <string_view>

namespace ui {
class SfmlRenderer : public IRenderer {
public:
    explicit SfmlRenderer(sf::RenderWindow& window);
    void render(const chess::IGame& game) override;
    sf::RenderWindow& getWindow() noexcept;

    // Selection highlight
    void setSelected(std::optional<chess::Square> sel);

    // Drag-and-drop support
    void startDrag(std::optional<chess::Square> source, sf::Vector2f mousePos);
    void updateDrag(sf::Vector2f mousePos);
    void stopDrag();

private:
    sf::RenderWindow& window_;
    std::optional<chess::Square> selected_;

    // Drag state
    std::optional<chess::Square> dragSource_;
    std::optional<sf::Vector2f> dragPosition_;
    bool dragging_{false};

    // Piece rendering: prefer texture atlas, fallback to font glyphs, else shapes.
    bool texturesLoaded_{false};
    sf::Texture piecesTexture_;
    std::array<sf::Sprite, 12> pieceSprites_; // 0..5 white Pawn..King, 6..11 black Pawn..King

    bool fontLoaded_{false};
    sf::Font font_;
    std::array<sf::Text, 12> pieceTexts_;

    // Helpers
    int spriteIndex(chess::PieceType type, chess::Color color) const noexcept;
    std::string unicodeGlyph(chess::PieceType type, chess::Color color) const noexcept;
    void ensureSpriteScale(float tileSize);
};
} // namespace ui
