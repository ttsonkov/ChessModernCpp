#pragma once
#include <memory>
#include "core/IGame.hpp"
#include "ui/IRenderer.hpp"
#include "ui/IInputHandler.hpp"

class Application {
public:
    Application(std::unique_ptr<chess::IGame> game,
                std::unique_ptr<ui::IRenderer> renderer,
                std::unique_ptr<ui::IInputHandler> inputHandler);
    ~Application();
    
    void run();

private:
    std::unique_ptr<chess::IGame> game_;
    std::unique_ptr<ui::IRenderer> renderer_;
    std::unique_ptr<ui::IInputHandler> inputHandler_;
};
