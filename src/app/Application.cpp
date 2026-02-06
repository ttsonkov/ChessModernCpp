#include "Application.hpp"
#include "ui/IRenderer.hpp"
#include "ui/IInputHandler.hpp"
#include "ui/SfmlInputHandler.hpp"
#include "ui/SfmlRenderer.hpp"
#include <stdexcept>
#include <optional>

Application::Application(std::unique_ptr<chess::IGame> game,
                         std::unique_ptr<ui::IRenderer> renderer,
                         std::unique_ptr<ui::IInputHandler> inputHandler)
    : game_(std::move(game)),
      renderer_(std::move(renderer)),
      inputHandler_(std::move(inputHandler)) {
    if (!game_ || !renderer_ || !inputHandler_) {
        throw std::runtime_error("Application requires game, renderer, and input handler");
    }
}

Application::~Application() = default;

void Application::run() {
    // Try to get concrete types for animation rendering
    auto* animInput = dynamic_cast<ui::SfmlInputHandler*>(inputHandler_.get());
    auto* animRenderer = dynamic_cast<ui::SfmlRenderer*>(renderer_.get());

    while (inputHandler_->isRunning()) {
        inputHandler_->updateAnimation();
        if (auto move = inputHandler_->processInput()) {
            game_->makeMove(*move);
        }
        renderer_->render(*game_);

        // Draw animation if active
        if (animInput && animRenderer) {
            const auto& anim = animInput->getAnimation();
            if (anim && anim->active && anim->progress < 1.0f) {
                // Get piece at anim->from (or anim->to if move is done)
                const auto& board = game_->board();
                std::optional<chess::Piece> piece;
                if (anim->progress < 1.0f) {
                    piece = board.at(anim->from);
                } else {
                    piece = board.at(anim->to);
                }
                if (piece) {
                    const auto size = animRenderer->getWindow().getSize();
                    float tile = std::min(size.x, size.y) / 8.f;
                    sf::Vector2f fromPx(anim->from.file * tile, anim->from.rank * tile);
                    sf::Vector2f toPx(anim->to.file * tile, anim->to.rank * tile);
                    sf::Vector2f pos = fromPx + (toPx - fromPx) * anim->progress;
                    animRenderer->drawPieceAt(*piece, pos, tile);
                }
            }
        }
    }
}
