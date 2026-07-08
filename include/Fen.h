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
}

