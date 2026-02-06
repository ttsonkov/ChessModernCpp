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

    /// Returns true if the input handler is in a running state
    /// and false otherwise. This can be used to control the main
    /// loop of the application.
    [[nodiscard]] virtual bool isRunning() const noexcept = 0;

    /// Update the animation state, if any.
    /// This is a no-op by default and can be overridden by derived classes.
    virtual void updateAnimation() {}
};

} // namespace ui
