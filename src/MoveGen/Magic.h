#pragma once

#include "Types/BitBoard.h"
#include "Types/Types.h"

namespace Magic {
BitBoard GetKingAttacks(Square sq, Square king, bool isRook);
template <PieceType>
BitBoard GetAttacks(Square sq, BitBoard occupied);
} // namespace Magic
