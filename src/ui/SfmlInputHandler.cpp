#include "SfmlInputHandler.hpp"
#include "Config.hpp"
#include <algorithm>
#include <chrono>
#include <ranges>

namespace ui {

// ============================================================================
// Construction
// ============================================================================

SfmlInputHandler::SfmlInputHandler(sf::RenderWindow& window) 
    : window_(window) {}

// ============================================================================
// IInputHandler Interface
// ============================================================================

bool SfmlInputHandler::isRunning() const noexcept {
    return running_ && window_.isOpen();
}

std::optional<chess::Move> SfmlInputHandler::processInput() {
    sf::Event event;

    // During animation, only handle close events
    if (animation_ && animation_->active) {
        while (window_.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                handleCloseEvent();
            }
        }
        return std::nullopt;
    }

    while (window_.pollEvent(event)) {
        switch (event.type) {
            case sf::Event::Closed:
                handleCloseEvent();
                break;

            case sf::Event::MouseButtonPressed:
                if (event.mouseButton.button == sf::Mouse::Left) {
                    handleMousePress(event.mouseButton.x, event.mouseButton.y);
                }
                break;

            case sf::Event::MouseMoved:
                handleMouseMove(event.mouseMove.x, event.mouseMove.y);
                break;

            case sf::Event::MouseButtonReleased:
                if (event.mouseButton.button == sf::Mouse::Left) {
                    handleMouseRelease(event.mouseButton.x, event.mouseButton.y);
                }
                break;

            default:
                break;
        }
    }

    return std::nullopt;
}

void SfmlInputHandler::setSelected(std::optional<chess::Square> square) {
    selected_ = square;
}

std::optional<chess::Move> SfmlInputHandler::updateAnimation() {
    if (!animation_ || !animation_->active) {
        return std::nullopt;
    }
    
    using namespace std::chrono;
    const auto now = steady_clock::now();
    const float elapsed = duration_cast<duration<float>>(now - animation_->start_time).count();
    const float progress = std::clamp(elapsed / animation_->duration, 0.0f, 1.0f);
    
    animation_->progress = progress;
    
    if (progress >= config::kAnimationCompleteThreshold) {
        auto completed_move = animation_->move;
        animation_.reset();
        return completed_move;
    }
    
    return std::nullopt;
}

std::optional<AnimationInfo> SfmlInputHandler::getAnimationState() const noexcept {
    if (!animation_ || !animation_->active) {
        return std::nullopt;
    }
    return AnimationInfo{
        .from = animation_->from,
        .to = animation_->to,
        .progress = animation_->progress,
        .active = animation_->active,
        .piece = animation_->piece
    };
}

// ============================================================================
// Animation Control
// ============================================================================

void SfmlInputHandler::startAnimation(chess::Square from, chess::Square to,
                                       chess::Piece piece, chess::Move move,
                                       float duration) {
    animation_ = AnimationState{
        .active = true,
        .from = from,
        .to = to,
        .piece = piece,
        .move = move,
        .progress = 0.0f,
        .start_time = std::chrono::steady_clock::now(),
        .duration = duration
    };
}

void SfmlInputHandler::stopAnimation() {
    animation_.reset();
}

// ============================================================================
// Event Handlers
// ============================================================================

void SfmlInputHandler::handleCloseEvent() {
    window_.close();
    running_ = false;
}

void SfmlInputHandler::handleMousePress(int x, int y) {
    auto sq = pixelToSquare(x, y);
    if (!sq || !game_ref_) {
        selected_.reset();
        clearLegalMoves();
        return;
    }

    // Only allow selecting pieces of the current player
    if (game_ref_->board().hasPieceAt(*sq, game_ref_->sideToMove())) {
        selected_ = sq;
        drag_source_ = sq;
        drag_position_ = sf::Vector2f(static_cast<float>(x), static_cast<float>(y));
        updateLegalMoves(*sq);
    } else {
        selected_.reset();
        clearLegalMoves();
    }
}

void SfmlInputHandler::handleMouseMove(int x, int y) {
    if (drag_source_) {
        drag_position_ = sf::Vector2f(static_cast<float>(x), static_cast<float>(y));
    }
}

void SfmlInputHandler::handleMouseRelease(int x, int y) {
    if (!drag_source_) return;

    auto target = pixelToSquare(x, y);
    
    if (target && *target != *drag_source_ && isLegalDestination(*target)) {
        if (const auto& piece_opt = game_ref_->board().at(*drag_source_)) {
            chess::Move move{.from = *drag_source_, .to = *target};
            startAnimation(*drag_source_, *target, *piece_opt, move);
        }
    }

    // Clean up drag state
    selected_.reset();
    drag_source_.reset();
    drag_position_.reset();
    clearLegalMoves();
}

// ============================================================================
// Coordinate Conversion
// ============================================================================

std::optional<chess::Square> SfmlInputHandler::pixelToSquare(int pixel_x, int pixel_y) const noexcept {
    const auto size = window_.getSize();
    const float tile = static_cast<float>(std::min(size.x, size.y)) / chess::kBoardSize;

    const int file = std::clamp(static_cast<int>(pixel_x / tile), 0, chess::kBoardSize - 1);
    const int rank = std::clamp(static_cast<int>(pixel_y / tile), 0, chess::kBoardSize - 1);

    return chess::Square{.rank = rank, .file = file};
}

// ============================================================================
// Legal Move Management
// ============================================================================

void SfmlInputHandler::updateLegalMoves(chess::Square from) {
    legal_move_squares_.clear();
    if (!game_ref_) return;

    const auto moves = game_ref_->legalMoves();
    for (const auto& move : moves) {
        if (move.from == from) {
            legal_move_squares_.push_back(move.to);
        }
    }
}

void SfmlInputHandler::clearLegalMoves() {
    legal_move_squares_.clear();
}

bool SfmlInputHandler::isLegalDestination(chess::Square sq) const noexcept {
    return std::ranges::find(legal_move_squares_, sq) != legal_move_squares_.end();
}

} // namespace ui
