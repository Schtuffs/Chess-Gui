#pragma once

#include "Constants.h"
#include "Board.h"
#include "Piece.h"

/**
 * @brief Generates moves for a given `Piece` and `Board` state.
 * @namespace MoveGen
 * @date 2026-06-29
 */
namespace MoveGen {
    /**
     * @brief Generates moves for a given `Piece` and `Board` state.
     * @param board The current `Board` state.
     * @param piece The `Piece` to generate moves for.
     * @return The bitboard representation of the moves.
     * @date 2026-06-29
     */
    BitBoard Generate(const Board& board, const Piece& piece);
}

