#include "MoveGen.h"

#include <cmath>
#include <print>

#include "Convert.h"
#include "Utils.h"



std::string u64ToString(BitBoard val, char on = '1', char off = '0')
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

/**
 * @return <0 on at least one piece is invalid. =0 on same colour. >0 on different colours.
 */
static int PieceCompare(const Piece& lhs, const Piece& rhs)
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

static BitBoard GenMove(const Board& board, const Piece& piece, i32 offset, Index mod)
{
    BitBoard bb = 0;
    DebugPrintln("\nPiece: {}, off: {}, mod: {}", piece.ToString(), offset, mod);
    for (Index i = 1; i < GRID_SIZE; i++) {
        Index index = piece.Position();
        index += (i32)i * offset;
        if (index >= GRID_SIZE * GRID_SIZE) {
            DebugPrintln("Index out of bounds: {}", index);
            break;
        }

        if ((index % (Index)GRID_SIZE) == mod) {
            DebugPrintln("Index is mod");
            break;
        }

        const Piece& other = board.Pieces()[index];
        DebugPrintln("{:2}: {}", index, other.ToString());
        int check = PieceCompare(piece, other);

        bool lastCheck = false;
        if (check == 0) {
            DebugPrintln("Equal colour");
            break;
        }
        else if (check > 0) {
            DebugPrintln("Opposite colour: Last check");
            lastCheck = true;
        }

        bb |= Convert::IndexToBitBoard(index);

        if (lastCheck) {
            break;
        }
    }

    return bb;
}

static BitBoard GenCardinal(const Board& board, const Piece& piece)
{
    BitBoard bb = Convert::IndexToBitBoard(piece.Position());

    bb |= GenMove(board, piece,  8, 0xff); // Up
    bb |= GenMove(board, piece,  1,    0); // Right
    bb |= GenMove(board, piece, -8, 0xff); // Down
    bb |= GenMove(board, piece, -1,    7); // Left

    return bb;
}

static BitBoard GenDiag(const Board& board, const Piece& piece)
{
    BitBoard bb = Convert::IndexToBitBoard(piece.Position());

    bb |= GenMove(board, piece,  7, 7); // Up left
    bb |= GenMove(board, piece,  9, 0); // Up right
    bb |= GenMove(board, piece, -7, 0); // Down right
    bb |= GenMove(board, piece, -9, 7); // Down left

    return bb;
}

static BitBoard GenSliding(const Board& board, const Piece& piece, bool isRook)
{
    if (isRook) {
        return GenCardinal(board, piece);
    }
    else {
        return GenDiag(board, piece);
    }
}

static BitBoard GenKnight(const Board& board, const Piece& piece)
{
    constexpr Index assumedStart = 18;
    BitBoard bb = 0x00'00'00'0a'11'00'11'0a;
    
    Index logpos = piece.Position();
    if (logpos % GRID_SIZE == 1) {
        bb &= 0x00'00'00'0a'10'00'10'0a;
    }
    else if (logpos % GRID_SIZE == 0) {
        bb &= 0x00'00'00'08'10'00'10'08;
    }
    else if (logpos % GRID_SIZE == GRID_SIZE - 2) {
        bb &= 0x00'00'00'0a'01'00'01'0a;
    }
    else if (logpos % GRID_SIZE == GRID_SIZE - 1) {
        bb &= 0x00'00'00'02'01'00'01'02;
    }
    
    int shift = logpos - assumedStart;
    if (shift > 0) {
        bb = (bb << shift);
    }
    else {
        bb = (bb >> (std::abs(shift)));
    }

    BitBoard index = 0;
    BitBoard pos = bb;
    while (pos) {
        if (pos & 1) {
            const Piece& other = board.Pieces()[index];
            int check = PieceCompare(piece, other);
            if (check == 0) {
                bb &= ~((BitBoard)1 << index);
            }
        }

        index++;
        pos = bb >> index;
    }
    
    return (bb | Convert::IndexToBitBoard(piece.Position()));
}

static BitBoard GenCastling(const Board& board)
{
    u8 castling = board.Castling(board.Player());
    BitBoard bb = 0;
    if (board.Player() == Enums::Colour::White) {
        if (castling & (u8)Enums::Castling::White_King) {
            const Piece& bishop = board.Pieces()[5];
            const Piece& knight = board.Pieces()[6];

            if (!bishop.IsValid() && !knight.IsValid()) {
                bb |= 0x00'00'00'00'00'00'00'40;
            }
        }
        
        if (castling & (u8)Enums::Castling::White_Queen) {
            const Piece& queen  = board.Pieces()[3];
            const Piece& bishop = board.Pieces()[2];
            const Piece& knight = board.Pieces()[1];
    
            if (!bishop.IsValid() && !knight.IsValid() && !queen.IsValid()) {
                bb |= 0x00'00'00'00'00'00'00'04;
            }
        }
    }
    else if (board.Player() == Enums::Colour::Black) {
        if (castling & (u8)Enums::Castling::Black_King) {
            const Piece& bishop = board.Pieces()[61];
            const Piece& knight = board.Pieces()[62];

            if (!bishop.IsValid() && !knight.IsValid()) {
                bb |= 0x40'00'00'00'00'00'00'00;
            }
        }
        
        if (castling & (u8)Enums::Castling::Black_Queen) {
            const Piece& queen  = board.Pieces()[59];
            const Piece& bishop = board.Pieces()[58];
            const Piece& knight = board.Pieces()[57];
    
            if (!bishop.IsValid() && !knight.IsValid() && !queen.IsValid()) {
                bb |= 0x04'00'00'00'00'00'00'00;
            }
        }
    }

    return bb;
}

static BitBoard GenKing(const Board& board, const Piece& piece)
{
    Index pos = piece.Position();
    BitBoard bb = Convert::IndexToBitBoard(pos);
    for (int rank = -1; rank < 2; rank++) {
        if (rank == -1 && (pos / GRID_SIZE) == 0) {
            continue;
        }

        if (rank == 1 && (pos / GRID_SIZE) == GRID_SIZE - 1) {
            continue;
        }
        
        for (int file = -1; file < 2; file++) {
            if (file == -1 && (pos % GRID_SIZE) == 0) {
                continue;
            }
    
            if (file == 1 && (pos % GRID_SIZE) == GRID_SIZE - 1) {
                continue;
            }

            BitBoard index = (BitBoard)((i64)pos + (rank * (i64)GRID_SIZE) + file);
            const Piece& other = board.Pieces()[index];
            int check = PieceCompare(piece, other);
            if (check == 0) {
                continue;
            }

            bb |= ((BitBoard)1 << index);
        }
    }
    
    return bb | GenCastling(board);
}

static BitBoard GenPawn(const Board& board, const Piece& piece)
{
    BitBoard logPos = piece.Position();
    BitBoard pos = Convert::IndexToBitBoard(logPos);
    BitBoard bb = pos;

    if (piece.Colour() == Enums::Colour::White) {
        // Move forward
        if (PieceCompare(piece, board.Pieces()[logPos + GRID_SIZE]) < 0) {
            bb |= pos << GRID_SIZE;
            if (logPos / GRID_SIZE == 1) {
                // Double move
                if (PieceCompare(piece, board.Pieces()[logPos + (GRID_SIZE * 2)]) < 0) {
                    bb |= pos << (GRID_SIZE * 2);
                }
            }
        }

        // Left attack
        if (PieceCompare(piece, board.Pieces()[logPos + GRID_SIZE - 1]) > 0) {
            bb |= (pos << (GRID_SIZE - 1));
        }
        // Right attack
        if (PieceCompare(piece, board.Pieces()[logPos + GRID_SIZE + 1]) > 0) {
            bb |= (pos << (GRID_SIZE + 1));
        }
        
        return bb;
    }
    
    if (piece.Colour() == Enums::Colour::Black) {
        // Move forward
        if (PieceCompare(piece, board.Pieces()[logPos - GRID_SIZE]) < 0) {
            bb |= pos >> GRID_SIZE;
            if (logPos / GRID_SIZE == 6) {
                // Double move
                if (PieceCompare(piece, board.Pieces()[logPos - (GRID_SIZE * 2)]) < 0) {
                    bb |= pos >> (GRID_SIZE * 2);
                }
            }
        }

        // Left attack
        if (PieceCompare(piece, board.Pieces()[logPos - (GRID_SIZE - 1)]) > 0) {
            bb |= (pos >> (GRID_SIZE - 1));
        }
        // Right attack
        if (PieceCompare(piece, board.Pieces()[logPos - (GRID_SIZE + 1)]) > 0) {
            bb |= (pos >> (GRID_SIZE + 1));
        }

        return bb;
    }

    return pos;
}

BitBoard MoveGen::Generate(const Board& board, const Piece& piece)
{
    BitBoard bb = 0;
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

