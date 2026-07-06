#pragma once

#include <string>
#include <string_view>

#include "Constants.h"

/**
 * @brief Helpful utilities for dealing with `Fen` items.
 * @namespace Fen
 * @date 2026-07-01
 */
namespace Fen {
    /**
     * @brief Checks if given fen is valid.
     * @param data The fen data to check.
     * @return `true` on valid fen.
     * @date 2026-07-01
     */
    bool IsValidFen(const char* data);

    /**
     * @brief Changes an index to a string move.
     * @param index The index to convert.
     * @return The move. Ex: 12 -> e2.
     * @date 2026-07-01
     */
    std::string IndexToMove(u8 index);
    
    /**
     * @brief Changes a string move to an index.
     * @param move The move to convert.
     * @return `true` on valid fen.
     * @return The index. Ex: e2 -> 12.
     * @date 2026-07-01
     */
    u8 MoveToIndex(std::string_view move);
}

