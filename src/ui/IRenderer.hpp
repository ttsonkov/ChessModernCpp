#pragma once
#include <span>
#include <vector>
#include "core/IGame.hpp"
#include "IInputHandler.hpp"

namespace ui {

/// Abstract interface for chess board rendering.
/// 
/// Implementations handle drawing:
/// - Board squares
/// - Pieces (static and animated)
/// - Selection and legal move highlights
class IRenderer {
public:
    virtual ~IRenderer() = default;
    
    // ========================================================================
    // Core Rendering
    // ========================================================================
    
    /// Render the current game state.
    /// @note Does not present to screen - call present() after all rendering.
    virtual void render(const chess::IGame& game) = 0;

    /// Present the rendered frame to the screen.
    /// @note Call after all rendering (including animations) is complete.
    virtual void present() noexcept {}
    
    // ========================================================================
    // Animation Support
    // ========================================================================

    /// Set the current animation state so renderer can hide the animating piece.
    /// @param animation Pointer to animation info, or nullptr to clear
    virtual void setAnimationState(const AnimationInfo* animation) noexcept { 
        (void)animation; 
    }

    /// Render a piece animation overlay.
    /// @param game The current game state
    /// @param animation The animation state to render
    virtual void renderPieceAnimation([[maybe_unused]] const chess::IGame& game,
                                      [[maybe_unused]] const AnimationInfo& animation) {}
    
    // ========================================================================
    // Highlighting
    // ========================================================================

    /// Set the legal move destinations to highlight.
    /// @param squares View of squares that are valid move destinations
    virtual void setLegalMoveHighlights(std::span<const chess::Square> squares) noexcept { 
        (void)squares; 
    }

    /// Clear legal move highlights.
    virtual void clearLegalMoveHighlights() noexcept {}
};

} // namespace ui
