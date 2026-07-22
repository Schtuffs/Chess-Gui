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

std::string_view Convert::CastleToMove(std::string_view move, Enums::Colour player)
{
    // Short castle
    if (move == "O-O") {
        if (player == Enums::Colour::White) {
            return "e1g1";
        }
        else if (player == Enums::Colour::Black) {
            return "e8g8";
        }
    }
    // Long castle
    else if (move == "O-O-O") {
        if (player == Enums::Colour::White) {
            return "e1c1";
        }
        else if (player == Enums::Colour::Black) {
            return "e8c8";
        }
    }

    return move;
}

std::string Convert::IndexToMove(Index index)
{
    return ((char)((index % (Index)8) + 'a') + std::to_string((index / (Index)8) + 1));
}

Index Convert::MoveToIndex(std::string_view move)
{
    return (Index)((move[1] - '1') * 8) + (move[0] - 'a');
}

std::string Convert::BitBoardToString(BitBoard val, char on, char off)
{
    const std::string VERT_SPACE = "\n +---+---+---+---+---+---+---+---+\n";
    const std::string HORZ_SPACE = " | ";
    std::string ret = "\n", line = HORZ_SPACE;
    
    for (int i = 0; i < 64; i++) {
        if ((val & ((BitBoard)1 << i)) > 0) {
            line += on + HORZ_SPACE;
        }
        else {
            line += off + HORZ_SPACE;
        }

        if (i != 0 && i % 8 == 7) {
            ret = line + VERT_SPACE + ret;
            line = HORZ_SPACE;
        }
    }

    ret = VERT_SPACE + ret;
    return ret;
}

std::string Convert::PiecesToString(const Piece* pieces)
{
    const std::string VERT_SPACE = "\n +---+---+---+---+---+---+---+---+\n";
    const std::string HORZ_SPACE = " | ";
    std::string ret = "\n", line = HORZ_SPACE;
    
    for (int i = 0; i < 64; i++) {
        const Piece& piece = pieces[i];
        if (!piece.IsValid()) {
            line += ' ' + HORZ_SPACE;
        }
        else {
            line += piece.AsChar() + HORZ_SPACE;
        }

        if (i != 0 && i % 8 == 7) {
            ret = line + VERT_SPACE + ret;
            line = HORZ_SPACE;
        }
    }

    ret = VERT_SPACE + ret;
    return ret;
}

