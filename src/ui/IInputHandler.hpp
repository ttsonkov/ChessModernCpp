#pragma once
#include <optional>
#include <span>
#include "core/Types.hpp"
#include "core/Move.hpp"

namespace ui {

/// Animation state for piece movement visualization.
///
/// Captures all information needed to render a moving piece
/// independently of the game state.
struct AnimationInfo {
    chess::Square from{};
    chess::Square to{};
    float progress{0.0f};  ///< 0.0 = start position, 1.0 = end position
    bool active{false};
    chess::Piece piece{};  ///< The piece being animated

    /// Check if animation has completed.
    [[nodiscard]] constexpr bool isComplete() const noexcept {
        return !active || progress >= 1.0f;
    }
};

/// Abstract interface for user input handling.
///
/// Implementations process platform-specific input events and produce chess moves.
/// Also manages animation state for smooth piece movement visualization.
class IInputHandler {
public:
    virtual ~IInputHandler() = default;

    // ========================================================================
    // Input Processing
    // ========================================================================

    /// Process pending input events.
    /// @return A move if one is ready, nullopt otherwise
    [[nodiscard]] virtual std::optional<chess::Move> processInput() = 0;

    /// Set the currently selected square for visual feedback.
    virtual void setSelected(std::optional<chess::Square> square) = 0;

    /// Check if the input handler is still running.
    /// @return false if the user requested to quit
    [[nodiscard]] virtual bool isRunning() const noexcept = 0;

    // ========================================================================
    // Animation
    // ========================================================================

    /// Update animation state and check for completion.
    /// @return The completed move if animation just finished, nullopt otherwise
    [[nodiscard]] virtual std::optional<chess::Move> updateAnimation() {
        return std::nullopt;
    }

    /// Get the current animation state for rendering.
    /// @return Animation info if active, nullopt otherwise
    [[nodiscard]] virtual std::optional<AnimationInfo> getAnimationState() const noexcept {
        return std::nullopt;
    }
};

} // namespace ui
