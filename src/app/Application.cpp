#include "Application.hpp"
#include <SFML/Window/Event.hpp>
#include "ui/SfmlRenderer.hpp"
#include "ui/SfmlInputHandler.hpp"

Application::Application(std::unique_ptr<chess::IGame> game,
                         std::unique_ptr<ui::IRenderer> renderer)
    : game_(std::move(game)), renderer_(std::move(renderer)) {
    // Create input handler from SfmlRenderer's window
    auto* sfml_renderer = dynamic_cast<ui::SfmlRenderer*>(renderer_.get());
    if (sfml_renderer) {
        inputHandler_ = std::make_unique<ui::SfmlInputHandler>(sfml_renderer->getWindow());
    }
}

Application::~Application() = default;

void Application::run() {
    auto* sfml_renderer = dynamic_cast<ui::SfmlRenderer*>(renderer_.get());
    if (!sfml_renderer || !inputHandler_) {
        throw std::runtime_error("Application requires SfmlRenderer and input handler");
    }

    auto& window = sfml_renderer->getWindow();

    while (window.isOpen()) {
        // Process input and get optional move
        if (auto move = inputHandler_->processInput()) {
            game_->makeMove(*move);
        }

        // Update renderer with selection state
        auto* input_handler = dynamic_cast<ui::SfmlInputHandler*>(inputHandler_.get());
        if (input_handler) {
            sfml_renderer->setSelected(input_handler->getSelected());
            sfml_renderer->setDragState(input_handler->getDragSource(), input_handler->getDragPosition());
        }

        // Render game state
        renderer_->render(*game_);
    }
}
