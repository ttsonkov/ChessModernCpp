#pragma once
#include <optional>
#include "core/Move.hpp"

namespace ui {

/// Handles user input and produces chess moves.
/// Decouples input handling from Application (Interface Segregation Principle).
class IInputHandler {
public:
    virtual ~IInputHandler() = default;

    /// Process user input and return optional move if one is ready.
    virtual std::optional<chess::Move> processInput() = 0;

    /// Set the currently selected square for visual feedback.
    virtual void setSelected(std::optional<chess::Square> square) = 0;
};

} // namespace ui
