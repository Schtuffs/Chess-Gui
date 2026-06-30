#pragma once

#include <string>
#include <string_view>

#include "Constants.h"

namespace Fen {
    bool IsValidFen(const char* data);
    std::string IndexToMove(u8 index);
    u8 MoveToIndex(std::string_view move);
}

