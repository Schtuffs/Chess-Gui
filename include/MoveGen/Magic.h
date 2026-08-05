#pragma once

#include "Constants.h"

namespace Magic {
    BitBoard GetSlidingAttacks(Index index, BitBoard blockers, bool isRook);
    BitBoard GetBishopAttacks(Index index, BitBoard blockers);
    BitBoard GetRookAttacks(Index index, BitBoard blockers);
} // namespace Magic
