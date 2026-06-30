#include "MoveGen.h"

#include <cmath>
#include <print>

#include "Utils.h"



/**
 * @return -1 on one piece is invalid. 0 on same colour. 1 on different colours.
 */
static int CheckPiece(const Piece& lhs, const Piece& rhs)
{
    if (!lhs.IsValid()) {
        return -1;
    }

    if (!rhs.IsValid()) {
        return -1;
    }

    Enums::Colour clhs = lhs.Colour();
    Enums::Colour crhs = rhs.Colour();

    if (clhs == crhs) {
        return 0;
    }
    
    return 1;
}

static u64 GenDiag(const Board& board, const Piece& piece, int offset, u64 mod, bool shiftLeft)
{
    (void)board;
    u64 bb = 0;
    for (u64 i = 0; i < GRID_SIZE; i++) {
        u64 pos = piece.Position();
        u64 tmp;

        if (shiftLeft) {
            tmp = (pos << (i * offset));
        } else {
            tmp = (pos >> (i * offset));
        }

        if ((u64)std::round(std::log2(tmp)) % GRID_SIZE == mod) {
            break;
        }

        bb |= tmp;
    }

    return bb;
}

static u64 GenDiag(const Board& board, const Piece& piece)
{
    u64 bb = piece.Position();

    bb |= GenDiag(board, piece, 7, 7, true);
    bb |= GenDiag(board, piece, 9, 0, true);
    bb |= GenDiag(board, piece, 7, 0, false);
    bb |= GenDiag(board, piece, 9, 7, false);

    return bb;
}

static u64 GenSliding(const Board& board, const Piece& piece, bool isRook)
{
    if (!isRook) {
        return GenDiag(board, piece);
    }

    u64 pos = piece.Position();
    u64 bb = pos;
    for (u64 i = 0; i < GRID_SIZE; i++) {
        if ((pos & ((u64)0xff << (i * 8))) > 0) {
            bb |= ((u64)0xff << (i * 8));
            break;
        }
    }

    u64 i = (u64)std::log2(pos);
    i = ((u64)1 << (i % 8));
    for (u64 j = 0; j < GRID_SIZE; j++) {
        bb |= (i << (j * GRID_SIZE));
    }

    return bb;
}

static u64 GenKnight(const Board& board, const Piece& piece)
{
    (void)board;
    u64 bb = piece.Position();
    return bb;
}

static u64 GenKing(const Board& board, const Piece& piece)
{
    (void)board;
    return piece.Position();
}

static u64 GenPawn(const Board& board, const Piece& piece)
{
    u64 pos = piece.Position();
    u64 logPos = (u64)std::log2(pos);
    u64 bb = pos;

    if (piece.Colour() == Enums::Colour::White) {
        if (CheckPiece(piece, board.Pieces()[logPos + GRID_SIZE]) < 0) {
            bb |= pos << GRID_SIZE;
            if (logPos / GRID_SIZE == 1) {
                if (CheckPiece(piece, board.Pieces()[logPos + (GRID_SIZE * 2)]) < 0) {
                    bb |= pos << (GRID_SIZE * 2);
                }
            }
        }
        return bb;
    }

    if (piece.Colour() == Enums::Colour::Black) {
        if (CheckPiece(piece, board.Pieces()[logPos - GRID_SIZE]) < 0) {
            bb |= pos >> GRID_SIZE;
            if (logPos / GRID_SIZE == 6) {
                if (CheckPiece(piece, board.Pieces()[logPos - (GRID_SIZE * 2)]) < 0) {
                    bb |= pos >> (GRID_SIZE * 2);
                }
            }
        }
        return bb;
    }

    return pos;
}

std::string u64ToString(u64 val, char on = '1', char off = '0')
{
    const std::string VERT_SPACE = "\n +---+---+---+---+---+---+---+---+\n";
    const std::string HORZ_SPACE = " | ";
    std::string ret = "\n", line = HORZ_SPACE;
    
    for (int i = 0; i < 64; i++) {
        if ((val & ((u64)1 << i)) > 0) {
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

u64 MoveGen::Generate(const Board& board, const Piece& piece)
{
    u64 bb = 0;
    if (!piece.IsValid()) {
        WarningPrintln("MoveGen::Generate: Invalid piece.");
        return bb;
    }

    switch (piece.Type()) {
    case Enums::Type::Bishop:
        bb = GenSliding(board, piece, false);
        break;
    case Enums::Type::King:
        bb = GenKing(board, piece);
        break;
    case Enums::Type::Knight:
        bb = GenKnight(board, piece);
        break;
    case Enums::Type::Pawn:
        bb = GenPawn(board, piece);
        break;
    case Enums::Type::Queen:
        bb = GenSliding(board, piece, false);
        bb |= GenSliding(board, piece, true);
        break;
    case Enums::Type::Rook:
        bb = GenSliding(board, piece, true);
        break;
    default:
        bb = 0;
        break;
    }

    DebugPrintln("{}", u64ToString(bb, 'X', ' '));
    
    return bb;
}

