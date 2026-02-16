#pragma once
#include "IRenderer.hpp"
#include "Config.hpp"
#include <SFML/Graphics.hpp>
#include <array>
#include <optional>
#include <string>
#include <vector>

namespace ui {

/// SFML-based chess board renderer.
///
/// Supports three rendering modes (in order of preference):
/// 1. Sprite textures from pieces.png
/// 2. Unicode chess glyphs (♔♕♖♗♘♙)
/// 3. Simple geometric shapes
class SfmlRenderer final : public IRenderer {
public:
    explicit SfmlRenderer(sf::RenderWindow& window);

    // IRenderer interface
    void render(const chess::IGame& game) override;
    void renderPieceAnimation(const chess::IGame& game, const AnimationInfo& animation) override;
    void setAnimationState(const AnimationInfo* animation) noexcept override;
    void setLegalMoveHighlights(std::span<const chess::Square> squares) noexcept override;
    void clearLegalMoveHighlights() noexcept override;
    void present() noexcept override;

    // Additional public interface
    [[nodiscard]] sf::RenderWindow& getWindow() noexcept;
    void setSelected(std::optional<chess::Square> sel) noexcept;
    void setDragState(std::optional<chess::Square> source, std::optional<sf::Vector2f> position) noexcept;

private:
    // Asset loading
    void loadPieceTextures();
    void loadFallbackFont();
    void initializeSpriteRects();

    // Sprite/glyph utilities
    [[nodiscard]] int spriteIndex(chess::PieceType type, chess::Color color) const noexcept;
    [[nodiscard]] std::string unicodeGlyph(chess::PieceType type, chess::Color color) const noexcept;
    void ensureSpriteScale(float tile_size);
    [[nodiscard]] float calculateTileSize() const noexcept;

    // Board drawing
    void drawBoard(float tile) noexcept;
    void drawSelectionHighlight(float tile) noexcept;
    void drawLegalMoveHighlights(const chess::IGame& game, float tile) noexcept;
    void drawMoveDot(sf::Vector2f center, float tile) noexcept;
    void drawCaptureRing(sf::Vector2f center, float tile) noexcept;

    // Piece drawing
    [[nodiscard]] bool shouldSkipPiece(chess::Square sq) const noexcept;
    void drawPieces(const chess::IGame& game, float tile);
    void drawPiece(const chess::Piece& piece, sf::Vector2f pos, float tile);
    void drawPieceSprite(const chess::Piece& piece, sf::Vector2f pos);
    void drawPieceGlyph(const chess::Piece& piece, sf::Vector2f pos, float tile);
    void drawFallbackPiece(const chess::Piece& piece, sf::Vector2f pos, float tile);
    void drawDraggedPiece(const chess::IGame& game, float tile);

    // State
    sf::RenderWindow& window_;
    std::optional<chess::Square> selected_;
    std::optional<chess::Square> drag_source_;
    std::optional<sf::Vector2f> drag_position_;
    const AnimationInfo* current_animation_{nullptr};
    std::vector<chess::Square> legal_move_squares_;

    // Texture-based rendering
    bool textures_loaded_{false};
    sf::Texture pieces_texture_;
    std::array<sf::Sprite, 12> piece_sprites_;

    // Font-based fallback rendering
    bool font_loaded_{false};
    sf::Font font_;
    std::array<sf::Text, 12> piece_texts_;
};

} // namespace ui
