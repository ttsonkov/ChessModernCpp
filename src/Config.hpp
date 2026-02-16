#pragma once
#include <string_view>

/// @file Config.hpp
/// @brief Application-wide configuration constants.

namespace config {

// ============================================================================
// Window Settings
// ============================================================================

inline constexpr int kWindowWidth = 800;
inline constexpr int kWindowHeight = 800;
inline constexpr int kFrameRateLimit = 60;
inline constexpr std::string_view kWindowTitle = "Modern Chess";

// ============================================================================
// Animation Settings
// ============================================================================

/// Duration of piece movement animation in seconds.
inline constexpr float kAnimationDuration = 0.15f;

/// Minimum animation progress to consider complete.
inline constexpr float kAnimationCompleteThreshold = 1.0f;

// ============================================================================
// Rendering Settings
// ============================================================================

/// Piece character size ratio relative to tile size (for font rendering).
inline constexpr float kPieceCharSizeRatio = 0.9f;

/// Fallback piece circle radius ratio relative to tile size.
inline constexpr float kFallbackPieceRadius = 0.38f;

/// Legal move dot radius ratio relative to tile size.
inline constexpr float kLegalMoveDotRadius = 0.15f;

/// Legal move capture ring radius ratio relative to tile size.
inline constexpr float kLegalMoveRingRadius = 0.45f;

/// Legal move capture ring thickness ratio relative to tile size.
inline constexpr float kLegalMoveRingThickness = 0.08f;

// ============================================================================
// Asset Paths
// ============================================================================

inline constexpr std::string_view kPiecesImagePath = "assets/pieces.png";
inline constexpr std::string_view kFallbackFontPath = "assets/DejaVuSans.ttf";

} // namespace config
