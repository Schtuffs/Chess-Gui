#pragma once

#include <string>
#include <string_view>

#include "raylib.h"

#include "Constants.h"
#include "Piece.h"

namespace Convert {
    /**
     * @brief Converts a `Color` to a ABGR u32.
     * @param col The `Color` to convert.
     * @return The ABGR u32 representation of the colour.
     * @date 2026-07-03
     */
    u32 ColorToU32(Color col);
    
    /**
     * @brief Converts a ABGR u32 to a `Color`.
     * @param col The ABGR u32 to convert.
     * @return The `Color` representation of the ABGR u32.
     * @date 2026-07-03
     */
    Color U32ToColor(u32 val);

    /**
     * @brief Changes an index to a string move.
     * @param index The index to convert.
     * @return The move. Ex: 12 -> e2.
     * @date 2026-07-01
     */
    std::string IndexToMove(Index index);
    
    /**
     * @brief Changes a string move to an index.
     * @param move The move to convert.
     * @return The index. Ex: e2 -> 12.
     * @date 2026-07-01
     */
    Index MoveToIndex(std::string_view move);

    /**
     * @brief Changes a grid index to a bitboard index.
     * @param index The index to convert.
     * @return The bitboard. Ex: 12 -> 0x00'00'00'00'00'00'10'00.
     * @date 2026-07-07
     */
    BitBoard IndexToBitBoard(Index index);
    
    /**
     * @brief Changes a bitboard index to a grid index.
     * @param bb The bitboard to convert.
     * @return The index. Ex: 0x00'00'00'00'00'00'10'00 -> 12.
     * @date 2026-07-07
     */
    Index BitBoardToIndex(BitBoard b);
    
    /**
     * @brief Changes a bitboard to a printable string.
     * @param bb The bitboard to convert.
     * @date 2026-07-07
     */
    std::string BitBoardToString(BitBoard b, char on = 'X', char off = ' ');
    
    /**
     * @brief Changes a bitboard to a printable string.
     * @param bb The bitboard to convert.
     * @date 2026-07-07
     */
    std::string PiecesToString(const Piece* pieces);
}

