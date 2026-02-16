#include "Application.hpp"
#include "ui/SfmlInputHandler.hpp"
#include <stdexcept>

namespace app {

// ============================================================================
// Construction
// ============================================================================

Application::Application(std::unique_ptr<chess::IGame> game,
                         std::unique_ptr<ui::IRenderer> renderer,
                         std::unique_ptr<ui::IInputHandler> input_handler)
    : game_(std::move(game)),
      renderer_(std::move(renderer)),
      input_handler_(std::move(input_handler)) {
    
    if (!game_ || !renderer_ || !input_handler_) {
        throw std::invalid_argument("Application requires non-null game, renderer, and input handler");
    }
    
    initializeInputHandler();
}

Application::~Application() = default;

void Application::initializeInputHandler() {
    // Inject game reference if this is an SfmlInputHandler
    if (auto* sfml_input = dynamic_cast<ui::SfmlInputHandler*>(input_handler_.get())) {
        sfml_input->setGameRef(game_.get());
    }
}

// ============================================================================
// Game Loop
// ============================================================================

void Application::run() {
    while (input_handler_->isRunning()) {
        processFrame();
    }
}

void Application::processFrame() {
    handleAnimation();
    handleInput();
    renderFrame();
}

void Application::handleAnimation() {
    if (auto completed_move = input_handler_->updateAnimation()) {
        game_->makeMove(*completed_move);
    }
}

void Application::handleInput() {
    if (auto move = input_handler_->processInput()) {
        game_->makeMove(*move);
    }
}

void Application::renderFrame() {
    auto anim_state = input_handler_->getAnimationState();
    
    // Configure renderer
    renderer_->setAnimationState(anim_state ? &(*anim_state) : nullptr);
    syncLegalMoveHighlights();

    // Render scene
    renderer_->render(*game_);
    
    if (anim_state) {
        renderer_->renderPieceAnimation(*game_, *anim_state);
    }

    // Present and cleanup
    renderer_->setAnimationState(nullptr);
    renderer_->present();
}

void Application::syncLegalMoveHighlights() {
    if (auto* sfml_input = dynamic_cast<ui::SfmlInputHandler*>(input_handler_.get())) {
        auto legal_squares = sfml_input->getLegalMoveSquares();
        if (!legal_squares.empty()) {
            renderer_->setLegalMoveHighlights(legal_squares);
        } else {
            renderer_->clearLegalMoveHighlights();
        }
    }
}

} // namespace app
