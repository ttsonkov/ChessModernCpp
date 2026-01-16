#include "Application.hpp"
#include <SFML/Window/Event.hpp>
#include <algorithm>
#include <optional>
#include "ui/SfmlRenderer.hpp"

Application::Application(std::unique_ptr<chess::IGame> game,
                         std::unique_ptr<ui::IRenderer> renderer)
    : game_(std::move(game)), renderer_(std::move(renderer)) {}

void Application::run() {
    auto* sr = dynamic_cast<ui::SfmlRenderer*>(renderer_.get());
    if (!sr) throw std::runtime_error("Application::run: renderer is not a ui::SfmlRenderer");

    auto& window = sr->getWindow();
    std::optional<chess::Square> selected;
    bool dragging = false;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
                continue;
            }

            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                const auto size = window.getSize();
                const auto tile = std::min(size.x, size.y) / 8.f;

                int file = static_cast<int>(event.mouseButton.x / tile);
                int rank = static_cast<int>(event.mouseButton.y / tile);
                file = std::clamp(file, 0, chess::Board::size - 1);
                rank = std::clamp(rank, 0, chess::Board::size - 1);

                const chess::Square clicked{rank, file};
                const auto& optPiece = game_->board().at(clicked);

                if (optPiece && optPiece->color == game_->sideToMove()) {
                    selected = clicked;
                    dragging = true;
                    sr->startDrag(selected, sf::Vector2f(static_cast<float>(event.mouseButton.x),
                                                         static_cast<float>(event.mouseButton.y)));
                } else {
                    selected.reset();
                    dragging = false;
                    sr->stopDrag();
                }
            }

            if (event.type == sf::Event::MouseMoved) {
                if (dragging && selected) {
                    sr->updateDrag(sf::Vector2f(static_cast<float>(event.mouseMove.x),
                                                static_cast<float>(event.mouseMove.y)));
                }
            }

            if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left) {
                const auto size = window.getSize();
                const auto tile = std::min(size.x, size.y) / 8.f;

                int file = static_cast<int>(event.mouseButton.x / tile);
                int rank = static_cast<int>(event.mouseButton.y / tile);
                file = std::clamp(file, 0, chess::Board::size - 1);
                rank = std::clamp(rank, 0, chess::Board::size - 1);

                const chess::Square released{rank, file};

                if (dragging && selected) {
                    if (released == *selected) {
                        selected.reset();
                    } else {
                        chess::Move move{*selected, released};
                        if (game_->makeMove(move)) selected.reset();
                    }
                }

                dragging = false;
                sr->stopDrag();
            }
        }

        sr->setSelected(selected);
        renderer_->render(*game_);
    }
}
