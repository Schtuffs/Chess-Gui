#pragma once

#include "Types/Position.h"
#include "Types/Types.h"

constexpr u8 MAX_MOVES = 255;

typedef struct MoveList {
    Move moves[MAX_MOVES];
    u8   size;
    MoveList() : size(0) {}

    void     Add(Move move) { moves[size++] = move; }
    void     Legalize(Position& pos);
    BitBoard ToBB(Square from) const noexcept;

    Move*       begin() noexcept;
    Move*       end() noexcept;
    const Move* begin() const noexcept;
    const Move* end() const noexcept;
} MoveList;

enum GenType { CAPTURES, QUIETS };

namespace MoveGen {
template <GenType>
void Generate(const Position& pos, MoveList& list);
void Generate(const Position& pos, MoveList& list);
}; // namespace MoveGen