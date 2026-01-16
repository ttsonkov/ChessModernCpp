#include <SFML/Graphics.hpp>
#include "core/ChessGame.hpp"
#include "ui/SfmlRenderer.hpp"
#include "app/Application.hpp"

int main() {
    sf::RenderWindow window({800, 800}, "Modern Chess");

    auto game = std::make_unique<chess::ChessGame>();
    auto renderer = std::make_unique<ui::SfmlRenderer>(window);

    Application app(std::move(game), std::move(renderer));
    app.run();
}
