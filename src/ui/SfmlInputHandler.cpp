#include "SfmlInputHandler.hpp"
#include <algorithm>
#include "core/Board.hpp"

namespace ui {

SfmlInputHandler::SfmlInputHandler(sf::RenderWindow& window) : window_(window) {}

std::optional<chess::Move> SfmlInputHandler::processInput() {
    sf::Event event;
    std::optional<chess::Move> move;

    while (window_.pollEvent(event)) {
        switch (event.type) {
            case sf::Event::Closed:
                window_.close();
                break;

            case sf::Event::MouseButtonPressed:
                if (event.mouseButton.button == sf::Mouse::Left) {
                    if (auto sq = pixelToSquare(event.mouseButton.x, event.mouseButton.y)) {
                        selected_ = sq;
                        dragSource_ = sq;
                        dragPosition_ = sf::Vector2f(static_cast<float>(event.mouseButton.x),
                                                     static_cast<float>(event.mouseButton.y));
                    } else {
                        selected_.reset();
                    }
                }
                break;

            case sf::Event::MouseMoved:
                if (dragSource_) {
                    dragPosition_ = sf::Vector2f(static_cast<float>(event.mouseMove.x),
                                                 static_cast<float>(event.mouseMove.y));
                }
                break;

            case sf::Event::MouseButtonReleased:
                if (event.mouseButton.button == sf::Mouse::Left && dragSource_) {
                    if (auto target = pixelToSquare(event.mouseButton.x, event.mouseButton.y)) {
                        if (*target != *dragSource_) {
                            move = chess::Move{*dragSource_, *target};
                        }
                    }
                    selected_.reset();
                    dragSource_.reset();
                    dragPosition_.reset();
                }
                break;

            default:
                break;
        }
    }

    return move;
}

void SfmlInputHandler::setSelected(std::optional<chess::Square> square) {
    selected_ = square;
}

std::optional<chess::Square> SfmlInputHandler::pixelToSquare(int pixelX, int pixelY) const {
    const auto size = window_.getSize();
    const auto tile = std::min(size.x, size.y) / 8.f;

    int file = static_cast<int>(pixelX / tile);
    int rank = static_cast<int>(pixelY / tile);

    file = std::clamp(file, 0, 7);
    rank = std::clamp(rank, 0, 7);

    return chess::Square{rank, file};
}

std::optional<chess::Square> SfmlInputHandler::getClickedSquare(int pixelX, int pixelY) {
    return pixelToSquare(pixelX, pixelY);
}

} // namespace ui
