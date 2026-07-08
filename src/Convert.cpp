#include "Convert.h"

#include <cmath>

u32 Convert::ColorToU32(Color col)
{
    return (col.a << 24) | (col.b << 16) | (col.g << 8) | col.r;
}

Color Convert::U32ToColor(u32 val)
{
    Color col;
    col.a = (0xff & (val >> 24));
    col.b = (0xff & (val >> 16));
    col.g = (0xff & (val >>  8));
    col.r = (0xff & (val >>  0));
    return col;
}

BitBoard Convert::IndexToBitBoard(Index index)
{
    return (BitBoard)1 << index;
}

Index Convert::BitBoardToIndex(BitBoard bb)
{
    return (Index)std::round(std::log2(bb));
}

std::string Convert::IndexToMove(Index index)
{
    return ((char)((index % GRID_SIZE) + 'a') + std::to_string((index / GRID_SIZE) + 1));
}

Index Convert::MoveToIndex(std::string_view move)
{
    return (Index)((move[1] - '1') * GRID_SIZE) + (move[0] - 'a');
}


