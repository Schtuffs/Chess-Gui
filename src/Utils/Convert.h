#pragma once

#include <cmath>
#include <string>
#include <string_view>

#include "raylib.h"

#include "State/Piece.h"
#include "Utils/Constants.h"

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
    constexpr u32 ColorToU32(Color col)
    {
        return (col.a << 24) | (col.b << 16) | (col.g << 8) | col.r;
    }

    /**
     * @brief Converts a ABGR u32 to a `Color`.
     * @param val The ABGR u32 to convert.
     * @return The `Color` representation of the ABGR u32.
     * @date 2026-07-03
     */
    constexpr Color U32ToColor(u32 val)
    {
        Color col;
        col.a = (0xff & (val >> 24));
        col.b = (0xff & (val >> 16));
        col.g = (0xff & (val >> 8));
        col.r = (0xff & (val >> 0));
        return col;
    }

    // Converts a letter to a piece type.
    constexpr Enums::Type CharToType(char c)
    {
        c = tolower(c);
        switch (c) {
        case 'b':
            return Enums::Type::Bishop;
        case 'k':
            return Enums::Type::King;
        case 'n':
            return Enums::Type::Knight;
        case 'p':
            return Enums::Type::Pawn;
        case 'q':
            return Enums::Type::Queen;
        case 'r':
            return Enums::Type::Rook;
        default:
            return Enums::Type::Invalid;
        }
    }

    /**
     * @brief Changes a `BitBoard` to an `Index`.
     * @param bb The `BitBoard` to convert.
     * @return The `Index`. Ex: 0x00'00'00'00'00'00'10'00 -> 12.
     * @date 2026-07-07
     */
    constexpr Index BitBoardToIndex(BitBoard bb) { return (Index)std::round(std::log2(bb)); }

    /**
     * @brief Changes an `Index` to a `BitBoard`.
     * @param index The `Index` to convert.
     * @return The `BitBoard`. Ex: 12 -> 0x00'00'00'00'00'00'10'00.
     * @date 2026-07-07
     */
    constexpr BitBoard IndexToBitBoard(Index index) { return (BitBoard)1 << index; }

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
} // namespace Convert
