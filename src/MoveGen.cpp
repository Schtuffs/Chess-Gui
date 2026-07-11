#include "MoveGen.h"

#include <cmath>
#include <print>

#include "Convert.h"
#include "Utils.h"



// ----- Creation / Destruction -----

MoveGen::MoveGen()
    : m_inCheck(false), m_inDoubleCheck(false), m_generatingAttacks(false)
{}



// ----- Update -----

BitBoard MoveGen::Generate(const Board& board, const Piece& piece)
{
    m_board = &board;

    BitBoard bb = GenPawn(piece);
    if (!piece.IsValid()) {
        WarningPrintln("MoveGen::Generate: Invalid piece.");
        return MoveGen::INVALID;
    }

    BitBoard attacks = GenAttacks();

    // Only generate king moves in double check
    if (m_inDoubleCheck) {
        if (piece.Type() != Enums::Type::King) {
            return Convert::IndexToBitBoard(piece.Position());
        }

        return GenKing(piece);
    }

    DebugPrintln("{}", Convert::BitBoardToString(bb));
    return bb;
}

// ----- Update ----- Hidden -----

/**
 * >0 Differing colours
 * =0 Same colours
 * <0 Invalid piece
 */
static int PieceCompare(const Piece& lhs, const Piece& rhs)
{
    constexpr int INVALID   = -1;
    constexpr int EQUAL     = 0;
    constexpr int OPPOSITE  = 1;

    if (!lhs.IsValid()) {
        return INVALID;
    }

    if (!rhs.IsValid()) {
        return INVALID;
    }

    if (lhs.Colour() == rhs.Colour()) {
        return EQUAL;
    }
    
    return OPPOSITE;
}

/**
 * >0 Differing colours
 * =0 Same colours
 * <0 Invalid piece
 */
int MoveGen::AddMove(const Piece& piece, Index index, BitBoard& bb)
{
    const Piece& other = m_board->Pieces()[index];
    if (PieceCompare(piece, other))
}

BitBoard MoveGen::GenAttacks()
{
    m_generatingAttacks = true;

    BitBoard bb = 0;
    const Piece* pieces = m_board->Pieces();
    
    for (Index i = 0; i < GRID_SIZE * GRID_SIZE; i++) {
        const Piece& piece = pieces[i];
        if (!piece.IsValid() || piece.Colour() == m_board->Player()) {
            continue;
        }

        bb |= GenTypeSelection(piece);
    }

    m_generatingAttacks = false;

    return MoveGen::INVALID;
}

BitBoard MoveGen::GenBishop(const Piece& piece)
{
    return Convert::IndexToBitBoard(piece.Position());
}

BitBoard MoveGen::GenRook(const Piece& piece)
{
    return Convert::IndexToBitBoard(piece.Position());
}

BitBoard MoveGen::GenQueen(const Piece& piece)
{
    return Convert::IndexToBitBoard(piece.Position());
}

BitBoard MoveGen::GenKing(const Piece& piece)
{
    return Convert::IndexToBitBoard(piece.Position());
}

BitBoard MoveGen::GenKnight(const Piece& piece)
{
    return Convert::IndexToBitBoard(piece.Position());
}

BitBoard MoveGen::GenPawn(const Piece& piece)
{
    return Convert::IndexToBitBoard(piece.Position());
}

BitBoard MoveGen::GenPawnAttack(const Piece& piece)
{
    return Convert::IndexToBitBoard(piece.Position());
}


/*

static BitBoard AddMove(const Board& board, const Piece& piece, Index index, bool isAttack)
{

}



static BitBoard GenSliding(const Board& board, const Piece& piece, i32 offset, Index mod, bool isAttack)
{
    BitBoard bb = 0;
    for (Index i = 1; i < GRID_SIZE; i++) {
        Index index = piece.Position();
        index += (i32)i * offset;
        if (index >= GRID_SIZE * GRID_SIZE) {
            DebugPrintln("MoveGen::GenSliding: Index out of bounds: {}", index);
            break;
        }

        if ((index % (Index)GRID_SIZE) == mod) {
            DebugPrintln("MoveGen::GenSliding: Index is mod");
            break;
        }

        const Piece& other = board.Pieces()[index];
        int check = PieceCompare(piece, other);

        bool lastCheck = false;
        if (check == 0) {
            DebugPrintln("MoveGen::GenSliding: Same colour");
            if (isAttack) {
                bb |= Convert::IndexToBitBoard(index);
            }
            break;
        }
        else if (check > 0) {
            DebugPrintln("MoveGen::GenSliding: Opposite colour: Last check");
            lastCheck = true;
        }

        bb |= Convert::IndexToBitBoard(index);

        if (lastCheck) {
            break;
        }
    }

    return bb;
}

static BitBoard GenBishop(const Board& board, const Piece& piece, bool isAttack)
{
    BitBoard bb = (isAttack ? 0 : Convert::IndexToBitBoard(piece.Position()));

    bb |= GenSliding(board, piece,  7,    7, isAttack); // Up left
    bb |= GenSliding(board, piece,  9,    0, isAttack); // Up right
    bb |= GenSliding(board, piece, -7,    0, isAttack); // Down right
    bb |= GenSliding(board, piece, -9,    7, isAttack); // Down left
    
    return bb;
}

static BitBoard GenRook(const Board& board, const Piece& piece, bool isAttack)
{
    BitBoard bb = (isAttack ? 0 : Convert::IndexToBitBoard(piece.Position()));
    
    bb |= GenSliding(board, piece,  8, 0xff, isAttack); // Up
    bb |= GenSliding(board, piece,  1,    0, isAttack); // Right
    bb |= GenSliding(board, piece, -8, 0xff, isAttack); // Down
    bb |= GenSliding(board, piece, -1,    7, isAttack); // Left
    
    return bb;
}



static BitBoard GenKnight(const Board& board, const Piece& piece, bool isAttack)
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
            if (check == 0 && !isAttack) {
                bb &= ~((BitBoard)1 << index);
            }
        }

        index++;
        pos = bb >> index;
    }
    
    BitBoard startPos = (isAttack ? 0 : Convert::IndexToBitBoard(piece.Position()));
    return (bb | startPos);
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

static BitBoard GenKing(const Board& board, const Piece& piece, bool isAttack)
{
    Index pos = piece.Position();
    BitBoard bb = (isAttack ? 0 : Convert::IndexToBitBoard(pos));
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
            if (check == 0 && !isAttack) {
                continue;
            }

            bb |= ((BitBoard)1 << index);
        }
    }
    
    return bb | GenCastling(board);
}



static BitBoard GenPawnAttack(const Piece& piece, const Piece& other, Index index, bool isAttack)
{
    BitBoard bb = 0;

    if (isAttack) {
        bb |= Convert::IndexToBitBoard(index);
    }
    else if (PieceCompare(piece, other) > 0 || other.IsEnPassant()) {
        bb |= Convert::IndexToBitBoard(index);
    }

    return bb;
}

static BitBoard GenPawn(const Board& board, const Piece& piece, i64 offset, Index mod, bool isAttack)
{
    BitBoard bb = 0;
    Index pos = piece.Position();

    // Forward moves
    Index checkIndex = pos + offset;
    if (PieceCompare(piece, board.Pieces()[checkIndex]) < 0) {
        bb |= Convert::IndexToBitBoard(checkIndex);
        if ((pos / GRID_SIZE) == mod) {
            // Double move
            checkIndex = pos + (offset * 2);
            if (PieceCompare(piece, board.Pieces()[checkIndex]) < 0) {
                bb |= Convert::IndexToBitBoard(checkIndex);
            }
        }
    }

    // Left attack
    checkIndex = pos + (offset - 1);
    bb |= GenPawnAttack(piece, board.Pieces()[checkIndex], checkIndex, isAttack);

    // Right attack
    checkIndex = pos + (offset + 1);
    bb |= GenPawnAttack(piece, board.Pieces()[checkIndex], checkIndex, isAttack);

    return bb;
}

static BitBoard GenPawn(const Board& board, const Piece& piece, bool isAttack)
{
    Index pos = piece.Position();
    BitBoard bb = (isAttack ? 0 : Convert::IndexToBitBoard(pos));

    if (piece.Colour() == Enums::Colour::White) {
        bb |= GenPawn(board, piece,  (i64)GRID_SIZE, 1, isAttack);
    }
    
    if (piece.Colour() == Enums::Colour::Black) {
        bb |= GenPawn(board, piece, -(i64)GRID_SIZE, 6, isAttack);
    }

    return bb;
}



static BitBoard SelectGeneration(const Board& board, const Piece& piece, bool isAttack)
{
    BitBoard bb = 0;

    switch (piece.Type()) {
    case Enums::Type::Bishop:
        bb = GenBishop(board, piece, isAttack);
        break;
    case Enums::Type::King:
        bb = GenKing(board, piece, isAttack);
        break;
    case Enums::Type::Knight:
        bb = GenKnight(board, piece, isAttack);
        break;
    case Enums::Type::Pawn:
        bb = GenPawn(board, piece, isAttack);
        break;
    case Enums::Type::Queen:
        bb = GenBishop(board, piece, isAttack);
        bb |= GenRook(board, piece, isAttack);
        break;
    case Enums::Type::Rook:
        bb = GenRook(board, piece, isAttack);
        break;
    default:
        bb = 0;
        break;
    }

    return bb;
}

static BitBoard GenAttacks(const Board& board)
{
    Enums::Colour attackColour = (
        board.Player() == Enums::Colour::White ? Enums::Colour::Black : Enums::Colour::White
    );

    BitBoard bb = 0;
    for (Index i = 0; i < GRID_SIZE * GRID_SIZE; i++) {
        const Piece& piece = board.Pieces()[i];
        if (piece.Colour() != attackColour) {
            continue;
        }

        bb |= SelectGeneration(board, piece, true);
    }

    return bb;
}

static bool CheckForChecks(const Board& board, BitBoard attacks)
{
    for (Index i = 0; i < GRID_SIZE * GRID_SIZE; i++) {
        BitBoard bb = (BitBoard)1 << i;
        if ((attacks & bb) == 0) {
            continue;
        }

        const Piece& piece = board.Pieces()[i];
        if (piece.Type() == Enums::Type::King && piece.Colour() == board.Player()) {
            if (inCheck) {
                inDoubleCheck = true;
                return true;
            }
            inCheck = true;
        }
    }

    return inCheck;
}

BitBoard MoveGen::Generate(const Board& board, const Piece& piece)
{
    BitBoard bb = 0;
    if (!piece.IsValid()) {
        WarningPrintln("MoveGen::Generate: Invalid piece.");
        return 0;
    }
    
    inCheck = false;
    inDoubleCheck = false;
    BitBoard attacks = GenAttacks(board);
    bool inCheck = CheckForChecks(board, attacks);
    DebugPrintln("Check? {}, {}", inCheck, inDoubleCheck);

    bb = SelectGeneration(board, piece, false);

    if (piece.Type() == Enums::Type::King) {
        bb &= ~attacks;
        bb |= Convert::IndexToBitBoard(piece.Position());
    }

    DebugPrintln("{}", Convert::BitBoardToString(bb));
    
    return bb;
}
*/
