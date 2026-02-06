#include "SfmlRenderer.hpp"
#include <algorithm>
#include <iostream>
#include <fstream>
#include <optional>
#include <string>
#include <vector>
#include <string_view>

#ifdef _WIN32
#include <windows.h>
#endif

namespace ui {

static constexpr std::string_view kPiecesImagePath = "assets/pieces.png";
static constexpr std::string_view kFallbackFontPath = "assets/DejaVuSans.ttf";

static bool fileExists(std::string_view path) {
    std::ifstream f(std::string(path), std::ios::binary);
    return f.good();
}

// Try a set of likely locations for the asset without using std::filesystem to avoid
// MSVC std::filesystem portability issues in this project configuration.
static std::optional<std::string> findAsset(std::string_view relative) {
    const std::string rel{relative};
    std::vector<std::string> candidates;
    candidates.reserve(6);

    // Direct relative to working directory
    candidates.push_back(rel);

    // Common IDE/build-relative locations
    candidates.push_back("..\\" + rel);
    candidates.push_back("..\\..\\" + rel);
    candidates.push_back(".\\" + rel);

#ifdef _WIN32
    // Try executable directory
    char buf[MAX_PATH];
    if (GetModuleFileNameA(nullptr, buf, MAX_PATH)) {
        const std::string exe(buf);
        if (auto pos = exe.find_last_of("\\/" ); pos != std::string::npos) {
            const std::string exeDir = exe.substr(0, pos + 1);
            candidates.push_back(exeDir + rel);
            candidates.push_back(exeDir + "..\\" + rel);
        }
    }
#endif

    for (const auto& c : candidates) if (fileExists(c)) return c;
    return std::nullopt;
}

SfmlRenderer::SfmlRenderer(sf::RenderWindow& window) : window_(window) {
    // Diagnostic: show where the process is running from (Output or console)
    try { std::cerr << "SfmlRenderer: attempt to locate assets (cwd unknown here)\n"; } catch(...){}

    // Attempt to locate pieces.png in several likely locations
    if (auto p = findAsset(kPiecesImagePath)) {
        std::cerr << "SfmlRenderer: found pieces image at: " << *p << "\n";
        if (piecesTexture_.loadFromFile(*p)) {
            texturesLoaded_ = true;
            piecesTexture_.setSmooth(true);
            const auto texSize = piecesTexture_.getSize();
            const unsigned cellW = texSize.x / 6;
            const unsigned cellH = texSize.y / 2;
            for (int colorRow = 0; colorRow < 2; ++colorRow) {
                for (int col = 0; col < 6; ++col) {
                    const int idx = colorRow * 6 + col;
                    pieceSprites_[idx].setTexture(piecesTexture_);
                    pieceSprites_[idx].setTextureRect(sf::IntRect(col * static_cast<int>(cellW),
                                                                  colorRow * static_cast<int>(cellH),
                                                                  static_cast<int>(cellW),
                                                                  static_cast<int>(cellH)));
                }
            }
            std::cerr << "SfmlRenderer: pieces texture size = " << texSize.x << "x" << texSize.y
                      << " cell = " << cellW << "x" << cellH << "\n";
        } else {
            std::cerr << "SfmlRenderer: piecesTexture_.loadFromFile FAILED for: " << *p << "\n";
        }
    } else {
        std::cerr << "SfmlRenderer: pieces.png not found in known locations\n";
    }

    // Font fallback to render Unicode chess glyphs
    if (!texturesLoaded_) {
        if (auto f = findAsset(kFallbackFontPath)) {
            std::cerr << "SfmlRenderer: found fallback font at: " << *f << "\n";
            if (font_.loadFromFile(*f)) {
                fontLoaded_ = true;
                for (auto &t : pieceTexts_) t.setFont(font_);
            } else {
                std::cerr << "SfmlRenderer: font_.loadFromFile FAILED for: " << *f << "\n";
            }
        } else {
            std::cerr << "SfmlRenderer: fallback font not found\n";
        }
    }
}

int SfmlRenderer::spriteIndex(chess::PieceType type, chess::Color color) const noexcept {
    constexpr std::array<int,6> kSheetOrder = { 5, 3, 2, 4, 1, 0 }; // mapping for user's sheet
    const int base = (color == chess::Color::White) ? 0 : 6;
    const int idx = static_cast<int>(type);
    if (static_cast<std::size_t>(idx) >= kSheetOrder.size()) return base;
    return base + kSheetOrder[idx];
}

std::string SfmlRenderer::unicodeGlyph(chess::PieceType type, chess::Color color) const noexcept {
    const char32_t code = [&]{
        switch(type) {
            case chess::PieceType::King:   return color==chess::Color::White ? 0x2654u : 0x265Au;
            case chess::PieceType::Queen:  return color==chess::Color::White ? 0x2655u : 0x265Bu;
            case chess::PieceType::Rook:   return color==chess::Color::White ? 0x2656u : 0x265Cu;
            case chess::PieceType::Bishop: return color==chess::Color::White ? 0x2657u : 0x265Du;
            case chess::PieceType::Knight: return color==chess::Color::White ? 0x2658u : 0x265Eu;
            case chess::PieceType::Pawn:   return color==chess::Color::White ? 0x2659u : 0x265Fu;
        }
        return 0x2654u;
    }();

    std::string out;
    if (code <= 0x7F) out.push_back(static_cast<char>(code));
    else if (code <= 0x7FF) {
        out.push_back(static_cast<char>(0xC0 | ((code >> 6) & 0x1F)));
        out.push_back(static_cast<char>(0x80 | (code & 0x3F)));
    } else if (code <= 0xFFFF) {
        out.push_back(static_cast<char>(0xE0 | ((code >> 12) & 0x0F)));
        out.push_back(static_cast<char>(0x80 | ((code >> 6) & 0x3F)));
        out.push_back(static_cast<char>(0x80 | (code & 0x3F)));
    } else {
        out.push_back(static_cast<char>(0xF0 | ((code >> 18) & 0x07)));
        out.push_back(static_cast<char>(0x80 | ((code >> 12) & 0x3F)));
        out.push_back(static_cast<char>(0x80 | ((code >> 6) & 0x3F)));
        out.push_back(static_cast<char>(0x80 | (code & 0x3F)));
    }
    return out;
}

void SfmlRenderer::ensureSpriteScale(float tileSize) {
    if (!texturesLoaded_) return;
    const auto rect = pieceSprites_[0].getTextureRect();
    if (rect.width == 0 || rect.height == 0) return;
    const float sx = tileSize / static_cast<float>(rect.width);
    const float sy = tileSize / static_cast<float>(rect.height);
    for (auto &s : pieceSprites_) s.setScale(sx, sy);
}

void SfmlRenderer::setSelected(std::optional<chess::Square> sel) noexcept { selected_ = sel; }

void SfmlRenderer::setDragState(std::optional<chess::Square> source, std::optional<sf::Vector2f> position) noexcept {
    dragSource_ = source;
    dragPosition_ = position;
}

void SfmlRenderer::drawBoard(float tile) noexcept {
    for (int rank = 0; rank < 8; ++rank) {
        for (int file = 0; file < 8; ++file) {
            sf::RectangleShape rect(sf::Vector2f(tile, tile));
            rect.setPosition(file * tile, rank * tile);
            const bool dark = ((rank + file) % 2) == 1;
            rect.setFillColor(dark ? sf::Color(118, 150, 86) : sf::Color(238, 238, 210));
            window_.draw(rect);
        }
    }
}

void SfmlRenderer::drawSelectionHighlight(float tile) noexcept {
    if (!selected_) return;

    const auto s = *selected_;
    sf::RectangleShape highlight(sf::Vector2f(tile, tile));
    highlight.setPosition(s.file * tile, s.rank * tile);
    highlight.setFillColor(sf::Color(255, 255, 0, 100));
    highlight.setOutlineColor(sf::Color(200, 200, 0, 200));
    highlight.setOutlineThickness(2.f);
    window_.draw(highlight);
}

void SfmlRenderer::drawPieces(const chess::IGame& game, float tile) {
    for (int rank = 0; rank < chess::Board::size; ++rank) {
        for (int file = 0; file < chess::Board::size; ++file) {
            const chess::Square sq{rank, file};
            const auto& optPiece = game.board().at(sq);
            if (!optPiece) continue;

            // Skip if dragging this piece
            if (dragSource_ && dragSource_->rank == rank && dragSource_->file == file) {
                continue;
            }

            const auto& p = *optPiece;
            const int idx = spriteIndex(p.type, p.color);

            if (texturesLoaded_) {
                sf::Sprite spr = pieceSprites_[idx];
                spr.setPosition(file * tile, rank * tile);
                window_.draw(spr);
            } else if (fontLoaded_) {
                auto& txt = pieceTexts_[idx];
                txt.setString(unicodeGlyph(p.type, p.color));
                txt.setCharacterSize(static_cast<unsigned>(tile * 0.9f));
                const sf::FloatRect bounds = txt.getLocalBounds();
                txt.setOrigin(bounds.left + bounds.width / 2.f, bounds.top + bounds.height / 2.f);
                txt.setPosition(file * tile + tile / 2.f, rank * tile + tile / 2.f);
                txt.setFillColor(p.color == chess::Color::White ? sf::Color::White : sf::Color::Black);
                txt.setOutlineColor(sf::Color(50, 50, 50));
                txt.setOutlineThickness(2.f);
                window_.draw(txt);
            } else {
                sf::CircleShape piece(tile * 0.38f);
                piece.setOrigin(piece.getRadius(), piece.getRadius());
                piece.setPosition(file * tile + tile / 2.f, rank * tile + tile / 2.f);
                piece.setFillColor(p.color == chess::Color::White ? sf::Color::White : sf::Color(40, 40, 40));
                piece.setOutlineColor(sf::Color::Black);
                piece.setOutlineThickness(2.f);
                window_.draw(piece);

                if (p.type != chess::PieceType::Pawn) {
                    sf::RectangleShape crown(sf::Vector2f(tile * 0.45f, tile * 0.08f));
                    crown.setOrigin(crown.getSize().x / 2.f, crown.getSize().y / 2.f);
                    crown.setPosition(file * tile + tile / 2.f, rank * tile + tile * 0.28f);
                    crown.setFillColor(sf::Color(200, 200, 50));
                    window_.draw(crown);
                }
            }
        }
    }
}

void SfmlRenderer::drawDraggedPiece(const chess::IGame& game, float tile) {
    if (!dragSource_ || !dragPosition_) return;

    const auto& optPiece = game.board().at(*dragSource_);
    if (!optPiece) return;

    const auto& p = *optPiece;
    const int idx = spriteIndex(p.type, p.color);

    if (texturesLoaded_) {
        sf::Sprite spr = pieceSprites_[idx];
        const auto rect = spr.getGlobalBounds();
        spr.setOrigin(0.f, 0.f);
        spr.setPosition(*dragPosition_ - sf::Vector2f(rect.width / 2.f, rect.height / 2.f));
        window_.draw(spr);
    } else if (fontLoaded_) {
        auto& txt = pieceTexts_[idx];
        txt.setString(unicodeGlyph(p.type, p.color));
        txt.setCharacterSize(static_cast<unsigned>(tile * 0.9f));
        const sf::FloatRect bounds = txt.getLocalBounds();
        txt.setOrigin(bounds.left + bounds.width / 2.f, bounds.top + bounds.height / 2.f);
        txt.setPosition(*dragPosition_);
        txt.setFillColor(p.color == chess::Color::White ? sf::Color::White : sf::Color::Black);
        txt.setOutlineColor(sf::Color(50, 50, 50));
        txt.setOutlineThickness(2.f);
        window_.draw(txt);
    } else {
        sf::CircleShape piece(tile * 0.38f);
        piece.setOrigin(piece.getRadius(), piece.getRadius());
        piece.setPosition(*dragPosition_);
        piece.setFillColor(p.color == chess::Color::White ? sf::Color::White : sf::Color(40, 40, 40));
        piece.setOutlineColor(sf::Color::Black);
        piece.setOutlineThickness(2.f);
        window_.draw(piece);
    }
}

void SfmlRenderer::drawPieceAt(const chess::Piece& piece, sf::Vector2f pos, float tile) {
    const int idx = spriteIndex(piece.type, piece.color);
    if (texturesLoaded_) {
        sf::Sprite spr = pieceSprites_[idx];
        spr.setPosition(pos);
        window_.draw(spr);
    } else if (fontLoaded_) {
        auto& txt = pieceTexts_[idx];
        txt.setString(unicodeGlyph(piece.type, piece.color));
        txt.setCharacterSize(static_cast<unsigned>(tile * 0.9f));
        const sf::FloatRect bounds = txt.getLocalBounds();
        txt.setOrigin(bounds.left + bounds.width / 2.f, bounds.top + bounds.height / 2.f);
        txt.setPosition(pos.x + tile / 2.f, pos.y + tile / 2.f);
        txt.setFillColor(piece.color == chess::Color::White ? sf::Color::White : sf::Color::Black);
        txt.setOutlineColor(sf::Color(50, 50, 50));
        txt.setOutlineThickness(2.f);
        window_.draw(txt);
    } else {
        sf::CircleShape shape(tile * 0.38f);
        shape.setOrigin(shape.getRadius(), shape.getRadius());
        shape.setPosition(pos.x + tile / 2.f, pos.y + tile / 2.f);
        shape.setFillColor(piece.color == chess::Color::White ? sf::Color::White : sf::Color(40, 40, 40));
        shape.setOutlineColor(sf::Color::Black);
        shape.setOutlineThickness(2.f);
        window_.draw(shape);
    }
}

void SfmlRenderer::render(const chess::IGame& game) {
    window_.clear(sf::Color::Black);
    const auto size = window_.getSize();
    const float tile = std::min(size.x, size.y) / 8.f;
    ensureSpriteScale(tile);

    drawBoard(tile);
    drawSelectionHighlight(tile);
    drawPieces(game, tile);
    drawDraggedPiece(game, tile);

    window_.display();
}

sf::RenderWindow& SfmlRenderer::getWindow() noexcept { return window_; }

} // namespace ui
