#include <SFML/Graphics.hpp>
#include "Config.hpp"
#include "core/ChessGame.hpp"
#include "ui/SfmlRenderer.hpp"
#include "ui/SfmlInputHandler.hpp"
#include "app/Application.hpp"

int main() {
    sf::RenderWindow window(
        sf::VideoMode(config::kWindowWidth, config::kWindowHeight),
        std::string(config::kWindowTitle)
    );
    window.setFramerateLimit(config::kFrameRateLimit);

    auto game = std::make_unique<chess::ChessGame>();
    auto renderer = std::make_unique<ui::SfmlRenderer>(window);
    auto input_handler = std::make_unique<ui::SfmlInputHandler>(window);

    app::Application application(
        std::move(game),
        std::move(renderer),
        std::move(input_handler)
    );
    application.run();

    return 0;
}
