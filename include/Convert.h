#pragma once

#include <string>
#include <string_view>

#include "raylib.h"

#include "Constants.h"
#include "Piece.h"

/**
 * @brief Conversion utility
 * @namespace Convert
 * @date 2026-07-03
 */
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
     * @param val The ABGR u32 to convert.
     * @return The `Color` representation of the ABGR u32.
     * @date 2026-07-03
     */
    Color U32ToColor(u32 val);

    // Convert given castling move to actual move.
    std::string_view CastleToMove(std::string_view move, Enums::Colour player);

    /**
     * @brief Changes an `Index` to a string move.
     * @param index The `Index` to convert.
     * @return The move. Ex: 12 -> e2.
     * @date 2026-07-01
     */
    std::string IndexToMove(Index index);
    
    /**
     * @brief Changes a string move to an `Index`.
     * @param move The move to convert.
     * @return The `Index`. Ex: e2 -> 12.
     * @date 2026-07-01
     */
    Index MoveToIndex(std::string_view move);
    
    /**
     * @brief Changes a `BitBoard` to an `Index`.
     * @param bb The `BitBoard` to convert.
     * @return The `Index`. Ex: 0x00'00'00'00'00'00'10'00 -> 12.
     * @date 2026-07-07
     */
    Index BitBoardToIndex(BitBoard bb);

    /**
     * @brief Changes an `Index` to a `BitBoard`.
     * @param index The `Index` to convert.
     * @return The `BitBoard`. Ex: 12 -> 0x00'00'00'00'00'00'10'00.
     * @date 2026-07-07
     */
    BitBoard IndexToBitBoard(Index index);
    
    /**
     * @brief Changes a `BitBoard` to a printable string.
     * @param bb The `BitBoard` to convert.
     * @param on  The char to use when 1 is detected.
     * @param off The char to use when 0 is detected.
     * @return The string representation.
     * @date 2026-07-07
     */
    std::string BitBoardToString(BitBoard bb, char on = 'X', char off = ' ');
    
    /**
     * @brief Changes a `Piece` list to a printable string.
     * @param pieces The `Piece` list to convert. Must be 64 in size.
     * @return The string representation.
     * @date 2026-07-07
     */
    std::string PiecesToString(const Piece* pieces);
}

