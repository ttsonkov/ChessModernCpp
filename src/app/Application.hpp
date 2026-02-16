#pragma once
#include <memory>
#include "core/IGame.hpp"
#include "ui/IRenderer.hpp"
#include "ui/IInputHandler.hpp"

namespace app {

/// Main application class coordinating the game loop.
/// 
/// Connects game logic, rendering, and input handling layers.
/// Manages the main loop lifecycle and frame processing.
class Application final {
public:
    /// Construct application with required dependencies.
    /// @throws std::invalid_argument if any dependency is null
    Application(std::unique_ptr<chess::IGame> game,
                std::unique_ptr<ui::IRenderer> renderer,
                std::unique_ptr<ui::IInputHandler> input_handler);
    
    ~Application();
    
    // Non-copyable, non-movable (owns the game loop)
    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;
    Application(Application&&) = delete;
    Application& operator=(Application&&) = delete;
    
    /// Run the main game loop until quit is requested.
    void run();

private:
    void initializeInputHandler();
    void processFrame();
    void handleAnimation();
    void handleInput();
    void renderFrame();
    void syncLegalMoveHighlights();

    std::unique_ptr<chess::IGame> game_;
    std::unique_ptr<ui::IRenderer> renderer_;
    std::unique_ptr<ui::IInputHandler> input_handler_;
};

} // namespace app
