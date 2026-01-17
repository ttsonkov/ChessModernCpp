#pragma once
#include "IRenderer.hpp"
#include <SFML/Graphics.hpp>
#include <optional>
#include <array>
#include <string>
#include <string_view>

namespace ui {
class IInputHandler;

class SfmlRenderer : public IRenderer {
public:
    explicit SfmlRenderer(sf::RenderWindow& window);
    void render(const chess::IGame& game) override;
    sf::RenderWindow& getWindow() noexcept;

    // Selection highlight
    void setSelected(std::optional<chess::Square> sel) noexcept;

    // Drag state (called by input handler)
    void setDragState(std::optional<chess::Square> source, std::optional<sf::Vector2f> position) noexcept;

private:
    sf::RenderWindow& window_;
    std::optional<chess::Square> selected_;
    std::optional<chess::Square> dragSource_;
    std::optional<sf::Vector2f> dragPosition_;

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
    void drawBoard(float tile) noexcept;
    void drawSelectionHighlight(float tile) noexcept;
    void drawPieces(const chess::IGame& game, float tile);
    void drawDraggedPiece(const chess::IGame& game, float tile);
};
} // namespace ui
