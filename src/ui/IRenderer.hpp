#pragma once
#include "core/IGame.hpp"

namespace ui {
class IRenderer {
public:
    virtual ~IRenderer() = default;
    virtual void render(const chess::IGame& game) = 0;
};
} // namespace ui
