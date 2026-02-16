#include "SfmlRenderer.hpp"
#include "Config.hpp"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <ranges>
#include <string>

#ifdef _WIN32
#include <windows.h>
#endif

namespace ui {

// ============================================================================
// Constants
// ============================================================================

namespace {

// Board colors (sf::Color is not constexpr in SFML 2.x)
const sf::Color kLightSquareColor{238, 238, 210};
const sf::Color kDarkSquareColor{118, 150, 86};
const sf::Color kSelectionFillColor{255, 255, 0, 100};
const sf::Color kSelectionOutlineColor{200, 200, 0, 200};
const sf::Color kLegalMoveColor{100, 100, 100, 180};

// Sprite sheet layout
constexpr int kPieceTypesPerRow = 6;
constexpr int kColorRows = 2;

// Fallback rendering constants
constexpr float kCrownWidth = 0.45f;
constexpr float kCrownHeight = 0.08f;
constexpr float kCrownOffsetY = 0.28f;

// Maps PieceType enum to sprite sheet column order
constexpr std::array<int, 6> kSheetColumnOrder = {5, 3, 2, 4, 1, 0};

// Unicode chess piece codepoints (indexed by PieceType)
constexpr std::array<char32_t, 6> kWhitePieceGlyphs = {
    0x2659, 0x2658, 0x2657, 0x2656, 0x2655, 0x2654
};
constexpr std::array<char32_t, 6> kBlackPieceGlyphs = {
    0x265F, 0x265E, 0x265D, 0x265C, 0x265B, 0x265A
};

} // namespace

// ============================================================================
// Asset Loading Utilities
// ============================================================================

namespace {

[[nodiscard]] bool fileExists(std::string_view path) noexcept {
    std::ifstream file(std::string(path), std::ios::binary);
    return file.good();
}

[[nodiscard]] std::optional<std::string> findAsset(std::string_view relative_path) {
    const std::string rel{relative_path};
    
    std::vector<std::string> search_paths = {
        rel,
        ".\\" + rel,
        "..\\" + rel,
        "..\\..\\" + rel,
    };

#ifdef _WIN32
    char exe_path[MAX_PATH];
    if (GetModuleFileNameA(nullptr, exe_path, MAX_PATH)) {
        std::string exe_str(exe_path);
        if (auto pos = exe_str.find_last_of("\\/"); pos != std::string::npos) {
            auto exe_dir = exe_str.substr(0, pos + 1);
            search_paths.emplace_back(exe_dir + rel);
            search_paths.emplace_back(exe_dir + "..\\" + rel);
        }
    }
#endif

    auto it = std::ranges::find_if(search_paths, fileExists);
    return (it != search_paths.end()) ? std::optional(*it) : std::nullopt;
}

} // namespace

// ============================================================================
// Unicode/UTF-8 Utilities
// ============================================================================

namespace {

[[nodiscard]] char32_t pieceToUnicode(chess::PieceType type, chess::Color color) noexcept {
    const auto idx = static_cast<std::size_t>(type);
    if (idx >= kWhitePieceGlyphs.size()) return kWhitePieceGlyphs[0];
    return (color == chess::Color::White) ? kWhitePieceGlyphs[idx] : kBlackPieceGlyphs[idx];
}

[[nodiscard]] std::string unicodeToUtf8(char32_t cp) {
    std::string result;
    if (cp <= 0x7F) {
        result.push_back(static_cast<char>(cp));
    } else if (cp <= 0x7FF) {
        result.push_back(static_cast<char>(0xC0 | ((cp >> 6) & 0x1F)));
        result.push_back(static_cast<char>(0x80 | (cp & 0x3F)));
    } else if (cp <= 0xFFFF) {
        result.push_back(static_cast<char>(0xE0 | ((cp >> 12) & 0x0F)));
        result.push_back(static_cast<char>(0x80 | ((cp >> 6) & 0x3F)));
        result.push_back(static_cast<char>(0x80 | (cp & 0x3F)));
    } else {
        result.push_back(static_cast<char>(0xF0 | ((cp >> 18) & 0x07)));
        result.push_back(static_cast<char>(0x80 | ((cp >> 12) & 0x3F)));
        result.push_back(static_cast<char>(0x80 | ((cp >> 6) & 0x3F)));
        result.push_back(static_cast<char>(0x80 | (cp & 0x3F)));
    }
    return result;
}

} // namespace

// ============================================================================
// Coordinate Conversion
// ============================================================================

namespace {

[[nodiscard]] sf::Vector2f squareToPixel(chess::Square sq, float tile) noexcept {
    return {static_cast<float>(sq.file) * tile, static_cast<float>(sq.rank) * tile};
}

[[nodiscard]] sf::Vector2f squareCenter(chess::Square sq, float tile) noexcept {
    return {sq.file * tile + tile / 2.f, sq.rank * tile + tile / 2.f};
}

} // namespace

// ============================================================================
// SfmlRenderer - Construction
// ============================================================================

SfmlRenderer::SfmlRenderer(sf::RenderWindow& window) : window_(window) {
    loadPieceTextures();
    if (!textures_loaded_) {
        loadFallbackFont();
    }
}

// ============================================================================
// SfmlRenderer - Asset Loading
// ============================================================================

void SfmlRenderer::loadPieceTextures() {
    auto path = findAsset(config::kPiecesImagePath);
    if (!path) {
        std::cerr << "SfmlRenderer: pieces.png not found\n";
        return;
    }

    if (!pieces_texture_.loadFromFile(*path)) {
        std::cerr << "SfmlRenderer: failed to load " << *path << "\n";
        return;
    }

    textures_loaded_ = true;
    pieces_texture_.setSmooth(true);
    initializeSpriteRects();
}

void SfmlRenderer::initializeSpriteRects() {
    const auto tex_size = pieces_texture_.getSize();
    const int cell_w = static_cast<int>(tex_size.x) / kPieceTypesPerRow;
    const int cell_h = static_cast<int>(tex_size.y) / kColorRows;

    for (int row = 0; row < kColorRows; ++row) {
        for (int col = 0; col < kPieceTypesPerRow; ++col) {
            const int idx = row * kPieceTypesPerRow + col;
            piece_sprites_[idx].setTexture(pieces_texture_);
            piece_sprites_[idx].setTextureRect({col * cell_w, row * cell_h, cell_w, cell_h});
        }
    }
}

void SfmlRenderer::loadFallbackFont() {
    auto path = findAsset(config::kFallbackFontPath);
    if (!path) {
        std::cerr << "SfmlRenderer: fallback font not found\n";
        return;
    }

    if (!font_.loadFromFile(*path)) {
        std::cerr << "SfmlRenderer: failed to load font " << *path << "\n";
        return;
    }

    font_loaded_ = true;
    for (auto& text : piece_texts_) {
        text.setFont(font_);
    }
}

// ============================================================================
// SfmlRenderer - State Management
// ============================================================================

void SfmlRenderer::setAnimationState(const AnimationInfo* animation) noexcept {
    current_animation_ = animation;
}

void SfmlRenderer::setLegalMoveHighlights(std::span<const chess::Square> squares) noexcept {
    legal_move_squares_.assign(squares.begin(), squares.end());
}

void SfmlRenderer::clearLegalMoveHighlights() noexcept {
    legal_move_squares_.clear();
}

void SfmlRenderer::setSelected(std::optional<chess::Square> sel) noexcept {
    selected_ = sel;
}

void SfmlRenderer::setDragState(std::optional<chess::Square> source,
                                std::optional<sf::Vector2f> position) noexcept {
    drag_source_ = source;
    drag_position_ = position;
}

// ============================================================================
// SfmlRenderer - Utilities
// ============================================================================

int SfmlRenderer::spriteIndex(chess::PieceType type, chess::Color color) const noexcept {
    const int color_offset = (color == chess::Color::White) ? 0 : kPieceTypesPerRow;
    const auto type_idx = static_cast<std::size_t>(type);
    if (type_idx >= kSheetColumnOrder.size()) return color_offset;
    return color_offset + kSheetColumnOrder[type_idx];
}

std::string SfmlRenderer::unicodeGlyph(chess::PieceType type, chess::Color color) const noexcept {
    return unicodeToUtf8(pieceToUnicode(type, color));
}

void SfmlRenderer::ensureSpriteScale(float tile_size) {
    if (!textures_loaded_) return;
    
    const auto rect = piece_sprites_[0].getTextureRect();
    if (rect.width == 0 || rect.height == 0) return;
    
    const float scale = tile_size / static_cast<float>(rect.width);
    for (auto& sprite : piece_sprites_) {
        sprite.setScale(scale, scale);
    }
}

float SfmlRenderer::calculateTileSize() const noexcept {
    const auto size = window_.getSize();
    return static_cast<float>(std::min(size.x, size.y)) / chess::kBoardSize;
}

// ============================================================================
// SfmlRenderer - Board Drawing
// ============================================================================

void SfmlRenderer::drawBoard(float tile) noexcept {
    for (int rank = 0; rank < chess::kBoardSize; ++rank) {
        for (int file = 0; file < chess::kBoardSize; ++file) {
            const bool is_dark = ((rank + file) % 2) == 1;
            
            sf::RectangleShape square({tile, tile});
            square.setPosition(static_cast<float>(file) * tile, static_cast<float>(rank) * tile);
            square.setFillColor(is_dark ? kDarkSquareColor : kLightSquareColor);
            window_.draw(square);
        }
    }
}

void SfmlRenderer::drawSelectionHighlight(float tile) noexcept {
    if (!selected_) return;

    sf::RectangleShape highlight({tile, tile});
    highlight.setPosition(squareToPixel(*selected_, tile));
    highlight.setFillColor(kSelectionFillColor);
    highlight.setOutlineColor(kSelectionOutlineColor);
    highlight.setOutlineThickness(2.f);
    window_.draw(highlight);
}

void SfmlRenderer::drawLegalMoveHighlights(const chess::IGame& game, float tile) noexcept {
    for (const auto& sq : legal_move_squares_) {
        const auto center = squareCenter(sq, tile);
        const bool is_capture = game.board().hasPieceAt(sq);
        
        if (is_capture) {
            drawCaptureRing(center, tile);
        } else {
            drawMoveDot(center, tile);
        }
    }
}

void SfmlRenderer::drawMoveDot(sf::Vector2f center, float tile) noexcept {
    sf::CircleShape dot(tile * config::kLegalMoveDotRadius);
    dot.setOrigin(dot.getRadius(), dot.getRadius());
    dot.setPosition(center);
    dot.setFillColor(kLegalMoveColor);
    window_.draw(dot);
}

void SfmlRenderer::drawCaptureRing(sf::Vector2f center, float tile) noexcept {
    sf::CircleShape ring(tile * config::kLegalMoveRingRadius);
    ring.setOrigin(ring.getRadius(), ring.getRadius());
    ring.setPosition(center);
    ring.setFillColor(sf::Color::Transparent);
    ring.setOutlineColor(kLegalMoveColor);
    ring.setOutlineThickness(tile * config::kLegalMoveRingThickness);
    window_.draw(ring);
}

// ============================================================================
// SfmlRenderer - Piece Drawing
// ============================================================================

bool SfmlRenderer::shouldSkipPiece(chess::Square sq) const noexcept {
    const bool dragging = drag_source_.has_value() && *drag_source_ == sq;
    const bool animating = current_animation_ != nullptr && 
                           current_animation_->active && 
                           current_animation_->from == sq;
    return dragging || animating;
}

void SfmlRenderer::drawPieces(const chess::IGame& game, float tile) {
    for (int rank = 0; rank < chess::kBoardSize; ++rank) {
        for (int file = 0; file < chess::kBoardSize; ++file) {
            const chess::Square sq{rank, file};
            
            if (shouldSkipPiece(sq)) continue;
            
            const auto& piece_opt = game.board().at(sq);
            if (!piece_opt) continue;

            drawPiece(*piece_opt, squareToPixel(sq, tile), tile);
        }
    }
}

void SfmlRenderer::drawPiece(const chess::Piece& piece, sf::Vector2f pos, float tile) {
    if (textures_loaded_) {
        drawPieceSprite(piece, pos);
    } else if (font_loaded_) {
        drawPieceGlyph(piece, pos, tile);
    } else {
        drawFallbackPiece(piece, pos, tile);
    }
}

void SfmlRenderer::drawPieceSprite(const chess::Piece& piece, sf::Vector2f pos) {
    sf::Sprite sprite = piece_sprites_[spriteIndex(piece.type, piece.color)];
    sprite.setPosition(pos);
    window_.draw(sprite);
}

void SfmlRenderer::drawPieceGlyph(const chess::Piece& piece, sf::Vector2f pos, float tile) {
    auto& text = piece_texts_[spriteIndex(piece.type, piece.color)];
    
    text.setString(unicodeGlyph(piece.type, piece.color));
    text.setCharacterSize(static_cast<unsigned>(tile * config::kPieceCharSizeRatio));
    
    const auto bounds = text.getLocalBounds();
    text.setOrigin(bounds.left + bounds.width / 2.f, bounds.top + bounds.height / 2.f);
    text.setPosition(pos.x + tile / 2.f, pos.y + tile / 2.f);
    text.setFillColor(piece.color == chess::Color::White ? sf::Color::White : sf::Color::Black);
    text.setOutlineColor({50, 50, 50});
    text.setOutlineThickness(2.f);
    
    window_.draw(text);
}

void SfmlRenderer::drawFallbackPiece(const chess::Piece& piece, sf::Vector2f pos, float tile) {
    const auto center = sf::Vector2f(pos.x + tile / 2.f, pos.y + tile / 2.f);
    const auto fill = (piece.color == chess::Color::White) 
                      ? sf::Color::White : sf::Color(40, 40, 40);

    sf::CircleShape circle(tile * config::kFallbackPieceRadius);
    circle.setOrigin(circle.getRadius(), circle.getRadius());
    circle.setPosition(center);
    circle.setFillColor(fill);
    circle.setOutlineColor(sf::Color::Black);
    circle.setOutlineThickness(2.f);
    window_.draw(circle);

    if (piece.type != chess::PieceType::Pawn) {
        sf::RectangleShape crown({tile * kCrownWidth, tile * kCrownHeight});
        crown.setOrigin(crown.getSize().x / 2.f, crown.getSize().y / 2.f);
        crown.setPosition(center.x, pos.y + tile * kCrownOffsetY);
        crown.setFillColor({200, 200, 50});
        window_.draw(crown);
    }
}

void SfmlRenderer::drawDraggedPiece(const chess::IGame& game, float tile) {
    if (!drag_source_ || !drag_position_) return;

    const auto& piece_opt = game.board().at(*drag_source_);
    if (!piece_opt) return;

    const auto centered_pos = *drag_position_ - sf::Vector2f(tile / 2.f, tile / 2.f);
    drawPiece(*piece_opt, centered_pos, tile);
}

// ============================================================================
// SfmlRenderer - Animation
// ============================================================================

void SfmlRenderer::renderPieceAnimation(const chess::IGame& /*game*/, 
                                         const AnimationInfo& animation) {
    if (animation.isComplete()) return;

    const float tile = calculateTileSize();
    ensureSpriteScale(tile);

    const auto from_px = squareToPixel(animation.from, tile);
    const auto to_px = squareToPixel(animation.to, tile);
    const auto current_pos = from_px + (to_px - from_px) * animation.progress;

    drawPiece(animation.piece, current_pos, tile);
}

// ============================================================================
// SfmlRenderer - Main Render
// ============================================================================

void SfmlRenderer::render(const chess::IGame& game) {
    window_.clear(sf::Color::Black);
    
    const float tile = calculateTileSize();
    ensureSpriteScale(tile);

    drawBoard(tile);
    drawSelectionHighlight(tile);
    drawLegalMoveHighlights(game, tile);
    drawPieces(game, tile);
    drawDraggedPiece(game, tile);
}

void SfmlRenderer::present() noexcept {
    window_.display();
}

sf::RenderWindow& SfmlRenderer::getWindow() noexcept {
    return window_;
}

} // namespace ui
