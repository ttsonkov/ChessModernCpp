#pragma once
#include <memory>
#include "core/IGame.hpp"
#include "ui/IRenderer.hpp"

class Application {
public:
    Application(std::unique_ptr<chess::IGame> game,
                std::unique_ptr<ui::IRenderer> renderer);
    void run();
private:
    std::unique_ptr<chess::IGame> game_;
    std::unique_ptr<ui::IRenderer> renderer_;
};
