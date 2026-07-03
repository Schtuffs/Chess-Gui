#include "MoveGen.h"

#include <cmath>
#include <print>

#include "Utils.h"



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

/**
 * @return <0 on one piece is invalid. =0 on same colour. >0 on different colours.
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

static u64 GenMove(const Board& board, const Piece& piece, i32 offset, u64 mod, bool shiftLeft)
{
    u64 bb = 0;
    for (u64 i = 1; i < GRID_SIZE; i++) {
        u64 pos = piece.Position();
        u64 tmp;

        if (shiftLeft) {
            tmp = (pos << (u64)(i * offset));
        } else {
            tmp = (pos >> (u64)(i * offset));
        }

        u64 index = (u64)std::log2(tmp);
        const Piece& other = board.Pieces()[index];
        int check = CheckPiece(piece, other);

        bool lastCheck = false;
        if (check == 0) {
            DebugPrintln("Same");
            break;
        }
        else if (check > 0) {
            DebugPrintln("Opposite");
            lastCheck = true;
        }

        if ((index % GRID_SIZE) == mod) {
            break;
        }

        bb |= tmp;

        if (lastCheck) {
            break;
        }
    }

    return bb;
}

static u64 GenCardinal(const Board& board, const Piece& piece)
{
    u64 bb = piece.Position();

    bb |= GenMove(board, piece, GRID_SIZE,    0xffffffff, false);
    bb |= GenMove(board, piece,         1, GRID_SIZE - 1, false);
    bb |= GenMove(board, piece, GRID_SIZE,     GRID_SIZE,  true);
    bb |= GenMove(board, piece,         1,    0xffffffff,  true);

    return bb;
}

static u64 GenDiag(const Board& board, const Piece& piece)
{
    u64 bb = piece.Position();

    bb |= GenMove(board, piece, 7, 7, true);
    bb |= GenMove(board, piece, 9, 0, true);
    bb |= GenMove(board, piece, 7, 0, false);
    bb |= GenMove(board, piece, 9, 7, false);

    return bb;
}

static u64 GenSliding(const Board& board, const Piece& piece, bool isRook)
{
    if (isRook) {
        return GenCardinal(board, piece);
    }
    else {
        return GenDiag(board, piece);
    }

}

static u64 GenKnight(const Board& board, const Piece& piece)
{
    (void)board;
    constexpr u64 assumedStart = 18;
    u64 movebb = 0x00'00'00'0a'11'00'11'0a;
    
    u64 logpos = (u64)std::log2(piece.Position());
    if (logpos % GRID_SIZE == 1) {
        movebb &= 0x00'00'00'0a'10'00'10'0a;
    }
    else if (logpos % GRID_SIZE == 0) {
        movebb &= 0x00'00'00'08'10'00'10'08;
    }
    else if (logpos % GRID_SIZE == GRID_SIZE - 2) {
        movebb &= 0x00'00'00'0a'01'00'01'0a;
    }
    else if (logpos % GRID_SIZE == GRID_SIZE - 1) {
        movebb &= 0x00'00'00'02'01'00'01'02;
    }
    
    int shift = logpos - assumedStart;
    if (shift > 0) {
        movebb = (movebb << shift);
    }
    else {
        movebb = (movebb >> (std::abs(shift)));
    }
    
    return movebb;
}

static u64 GenKing(const Board& board, const Piece& piece)
{
    (void)board;
    constexpr u64 assumedStart = 9;
    u64 movebb = 0x00'00'00'00'00'07'07'07;
    
    u64 logpos = (u64)std::log2(piece.Position());
    if (logpos % GRID_SIZE == 0) {
        movebb &= 0x00'00'00'00'00'06'06'06;
    }
    else if (logpos % GRID_SIZE == GRID_SIZE - 1) {
        movebb &= 0x00'00'00'00'00'03'03'03;
    }

    int shift = logpos - assumedStart;
    if (shift > 0) {
        movebb = (movebb << shift);
    }
    else {
        movebb = (movebb >> (std::abs(shift)));
    }

    return movebb;
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

u64 MoveGen::Generate(const Board& board, const Piece& piece)
{
    u64 bb = 0;
    if (!piece.IsValid()) {
        WarningPrintln("MoveGen::Generate: Invalid piece.");
        return 0;
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

