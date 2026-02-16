#include "ChessGame.hpp"
#include <algorithm>
#include <ranges>

namespace chess {

// ============================================================================
// Construction
// ============================================================================

ChessGame::ChessGame() { 
    newGame(); 
}

void ChessGame::newGame() {
    board_.reset();
    side_to_move_ = Color::White;
    last_move_.reset();
    castling_rights_ = {true, true, true, true};
}

// ============================================================================
// Move Execution
// ============================================================================

bool ChessGame::makeMove(const Move& move) {
    auto moves = rules_.legalMoves(board_, side_to_move_, last_move_, castling_rights_);
    
    // Find matching legal move
    auto it = std::ranges::find_if(moves, [&](const Move& m) {
        return m.from == move.from && m.to == move.to;
    });
    
    if (it == moves.end()) {
        return false;
    }

    const Move legal_move = *it;
    
    handleSpecialMoves(legal_move);
    updateCastlingRights(legal_move);
    
    last_move_ = legal_move;
    side_to_move_ = opponent(side_to_move_);
    
    return true;
}

void ChessGame::handleSpecialMoves(const Move& move) {
    // En-passant: remove captured pawn
    if (move.en_passant) {
        board_.clearSquare({move.from.rank, move.to.file});
    }

    // Execute the main piece movement
    board_.movePiece(move);

    // Castling: also move the rook
    if (move.castling) {
        const int rank = move.from.rank;
        if (move.to.file == 6) {
            // Kingside: rook h -> f
            board_.movePiece({
                .from = {rank, 7}, 
                .to = {rank, 5}
            });
        } else if (move.to.file == 2) {
            // Queenside: rook a -> d
            board_.movePiece({
                .from = {rank, 0}, 
                .to = {rank, 3}
            });
        }
    }
}

void ChessGame::updateCastlingRights(const Move& move) {
    const auto& piece = board_.at(move.to);
    
    // King moved: revoke both castling rights for that color
    if (piece && piece->type == PieceType::King) {
        if (side_to_move_ == Color::White) {
            castling_rights_[0] = false;
            castling_rights_[1] = false;
        } else {
            castling_rights_[2] = false;
            castling_rights_[3] = false;
        }
    }

    // Rook moved from initial square: revoke corresponding right
    auto revokeIfRookSquare = [this](Square sq) {
        if (sq.rank == 7) {
            if (sq.file == 0) castling_rights_[1] = false;      // White queenside
            else if (sq.file == 7) castling_rights_[0] = false; // White kingside
        } else if (sq.rank == 0) {
            if (sq.file == 0) castling_rights_[3] = false;      // Black queenside
            else if (sq.file == 7) castling_rights_[2] = false; // Black kingside
        }
    };

    revokeIfRookSquare(move.from);
    revokeIfRookSquare(move.to);  // Also handles rook capture
}

// ============================================================================
// State Queries
// ============================================================================

Color ChessGame::sideToMove() const noexcept { 
    return side_to_move_; 
}

const Board& ChessGame::board() const noexcept { 
    return board_; 
}

std::vector<Move> ChessGame::legalMoves() const { 
    return rules_.legalMoves(board_, side_to_move_, last_move_, castling_rights_); 
}

// ============================================================================
// Game Status
// ============================================================================

bool ChessGame::isCheck() const {
    return rules_.isCheck(board_, side_to_move_);
}

bool ChessGame::isCheckmate() const {
    return rules_.isCheckmate(board_, side_to_move_, last_move_, castling_rights_);
}

bool ChessGame::isStalemate() const {
    return rules_.isStalemate(board_, side_to_move_, last_move_, castling_rights_);
}

bool ChessGame::isGameOver() const {
    return isCheckmate() || isStalemate();
}

} // namespace chess
