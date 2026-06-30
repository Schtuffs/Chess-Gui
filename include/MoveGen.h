#pragma once

#include "Constants.h"
#include "Board.h"
#include "Piece.h"

namespace MoveGen {
    u64 Generate(const Board& board, const Piece& piece);
}

