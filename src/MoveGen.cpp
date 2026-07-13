#include "MoveGen.h"

#include <cmath>
#include <print>

#include "Convert.h"
#include "Utils.h"

enum MoveResult {
    MOVE_CONTINUE = 1,
    MOVE_UNTIL_NEXT,
    MOVE_END,
    MOVE_CAPTURE_KING,
};



// ----- Creation / Destruction -----

MoveGen::MoveGen()
  : m_pieceList(nullptr), m_pieceIndex(INVALID),
    m_generatingAttacks(false), m_inCheck(false), m_inDoubleCheck(false), m_pinningPiece(false),
    m_pinIndex(INVALID),
    m_attacks(0), m_pinsHorz(0), m_pinsVert(0), m_pinsUp(0), m_pinsDown(0)
{}



// ----- Update -----

BitBoard MoveGen::Generate(const Piece* pieces, Index index)
{
    Reset();

    m_pieceList = pieces;
    m_pieceIndex = index;
    
    const Piece& piece = m_pieceList[m_pieceIndex];
    if (!piece.IsValid()) {
        WarningPrintln("MoveGen::Generate: Invalid piece.");
        return MoveGen::INVALID;
    }

    m_attacks = GenAttacks();
    DebugPrintln("{}", Convert::BitBoardToString(m_attacks));
    m_inDoubleCheck = true;

    if (m_inDoubleCheck) {
        if (piece.Type() == Enums::Type::King) {
            return GenMoves(piece);
        }
        return Convert::IndexToBitBoard(piece.Position());
    }

    return GenMoves(piece);
}

// ----- Update ----- Hidden -----

void MoveGen::Reset()
{
    m_inCheck = false;
    m_inDoubleCheck = false;
    m_generatingAttacks = false;
    m_pinningPiece = false;

    m_attacks  = 0;
    m_pinsHorz = 0;
    m_pinsVert = 0;
    m_pinsUp   = 0;
    m_pinsDown = 0;
}

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

bool MoveGen::CheckPin(const Piece& other)
{
    if (!m_generatingAttacks) {
        return false;
    }

    if (other.Type() == Enums::Type::King) {
        if (m_pinningPiece) {
            // m
        }
        if (m_inCheck) {
            m_inDoubleCheck = true;
        }
        m_inCheck = true;
    }
    return -1;
}

/**
 * Use `MoveResult` enum to determine outcome.
 */
int MoveGen::AddMove(const Piece& piece, const Piece& other, Index index, BitBoard& bb)
{
    int compare = PieceCompare(piece, other);

    if (compare < 0) {
        bb |= Convert::IndexToBitBoard(index);
        return MOVE_CONTINUE;
    }
    
    else if (compare == 0) {
        if (m_generatingAttacks) {
            bb |= Convert::IndexToBitBoard(index);
        }
        return MOVE_END;
    }
    
    else {
        // bool pinned = CheckPin(other);
        bb |= Convert::IndexToBitBoard(index);

        return MOVE_END;
    }
}

BitBoard MoveGen::GenAttacks()
{
    m_generatingAttacks = true;

    BitBoard bb = 0;
    const Piece* pieces = m_pieceList;
    Enums::Colour currentColour = m_pieceList[m_pieceIndex].Colour();
    
    for (Index i = 0; i < GRID_SIZE * GRID_SIZE; i++) {
        m_pinningPiece = false;
        
        const Piece& piece = pieces[i];
        if (!piece.IsValid() || piece.Colour() == currentColour) {
            continue;
        }

        bb |= GenMoves(piece);
    }

    m_generatingAttacks = false;

    return bb;
}

BitBoard MoveGen::GenSliding(const Piece& piece, i32 offset, Index mod)
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

        const Piece& other = m_pieceList[index];
        int res = AddMove(piece, other, index, bb);

        if (res == MOVE_END) {
            break;
        }
    }

    return bb;
}

BitBoard MoveGen::GenBishop(const Piece& piece)
{
    BitBoard bb = 0;

    bb |= GenSliding(piece,  7, 7); // Up left
    bb |= GenSliding(piece,  9, 0); // Up right
    bb |= GenSliding(piece, -7, 0); // Down right
    bb |= GenSliding(piece, -9, 7); // Down left
    
    return bb;
}

BitBoard MoveGen::GenRook(const Piece& piece)
{
    BitBoard bb = 0;
    
    bb |= GenSliding(piece,  8, 0xff); // Up
    bb |= GenSliding(piece,  1,    0); // Right
    bb |= GenSliding(piece, -8, 0xff); // Down
    bb |= GenSliding(piece, -1,    7); // Left
    
    return bb;
}

BitBoard MoveGen::GenQueen(const Piece& piece)
{
    BitBoard bb = 0;

    bb |= GenRook(piece);
    bb |= GenBishop(piece);

    return bb;
}

BitBoard MoveGen::GenKing(const Piece& piece)
{
    Index pos = piece.Position();
    BitBoard bb = 0;

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

            Index index = (Index)((Index)pos + (rank * (Index)GRID_SIZE) + file);
            if (m_generatingAttacks) {
                if (!(file == 0 && file == rank)) {
                    bb |= Convert::IndexToBitBoard(index);
                }
                continue;
            }

            const Piece& other = m_pieceList[index];
            int check = PieceCompare(piece, other);
            if (check == 0) {
                continue;
            }

            int res = AddMove(piece, other, index, bb);
            (void)res;

            // bb |= Convert::IndexToBitBoard(index);
        }
    }
    
    return bb;// | GenCastling(board);
}

BitBoard MoveGen::GenKnight(const Piece& piece)
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
            const Piece& other = m_pieceList[index];
            int res = AddMove(piece, other, index, bb);
            (void)res;
            // int check = PieceCompare(piece, other);
            // if (check == 0 && !isAttack) {
            //     bb &= ~((BitBoard)1 << index);
            // }
        }

        index++;
        pos = bb >> index;
    }
    
    return bb;
}

BitBoard MoveGen::GenPawn(const Piece& piece)
{
    const i8 offset = ((piece.Colour() == Enums::Colour::White) ? (i8)GRID_SIZE : (-(i8)GRID_SIZE));
    const Index rank = ((piece.Colour() == Enums::Colour::White) ? 1 : (GRID_SIZE - 2));

    BitBoard bb = 0;
    Index pos = piece.Position();
    Index checkIndex = pos + offset;

    // Forward moves
    const Piece& other = m_pieceList[checkIndex];
    if (AddMove(piece, other, checkIndex, bb) == MOVE_CONTINUE) {
        if ((pos / GRID_SIZE) == rank) {
            checkIndex += offset;
            AddMove(piece, other, checkIndex, bb);
        }
    }

    // Left attack
    checkIndex = pos + (offset - 1);
    AddMove(piece, m_pieceList[checkIndex], checkIndex, bb);
    // bb |= GenPawnAttack(piece, m_pieceList[checkIndex], checkIndex, isAttack);
    
    // Right attack
    checkIndex = pos + (offset + 1);
    AddMove(piece, m_pieceList[checkIndex], checkIndex, bb);
    // bb |= GenPawnAttack(piece, m_pieceList[checkIndex], checkIndex, isAttack);

    return bb;
    return 0;
}

BitBoard MoveGen::GenMoves(const Piece& piece)
{
    BitBoard bb = (m_generatingAttacks ? 0 : Convert::IndexToBitBoard(piece.Position()));

    switch (piece.Type()) {
    case Enums::Type::Bishop:
        bb |= GenBishop(piece);
        break;
    case Enums::Type::King:
        bb |= GenKing(piece);
        break;
    case Enums::Type::Knight:
        bb |= GenKnight(piece);
        break;
    case Enums::Type::Pawn:
        bb |= GenPawn(piece);
        break;
    case Enums::Type::Queen:
        bb |= GenQueen(piece);
        break;
    case Enums::Type::Rook:
        bb |= GenRook(piece);
        break;
    default:
        bb |= 0;
        break;
    }

    return bb;
}



/*
static BitBoard AddMove(const Piece& piece, Index index)
{

}



BitBoard MoveGen::GenBishop(const Piece& piece)
{
    BitBoard bb = 0;

    bb |= GenSliding(piece,  7, 7); // Up left
    bb |= GenSliding(piece,  9, 0); // Up right
    bb |= GenSliding(piece, -7, 0); // Down right
    bb |= GenSliding(piece, -9, 7); // Down left
    
    return bb;
}

BitBoard MoveGen::GenRook(const Piece& piece)
{
    BitBoard bb = 0;
    
    bb |= GenSliding(piece,  8, 0xff); // Up
    bb |= GenSliding(piece,  1,    0); // Right
    bb |= GenSliding(piece, -8, 0xff); // Down
    bb |= GenSliding(piece, -1,    7); // Left
    
    return bb;
}



BitBoard MoveGen::GenKnight(const Piece& piece)
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

BitBoard MoveGen::GenKing(const Piece& piece)
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



static BitBoard GenPawnAttack(const Piece& piece, const Piece& other, Index index)
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

static BitBoard GenPawn(const Piece& piece, i64 offset, Index mod, bool isAttack)
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

BitBoard MoveGen::GenPawn(const Piece& piece)
{
    Index pos = piece.Position();
    BitBoard bb = (m_generatingAttacks ? 0 : Convert::IndexToBitBoard(pos));

    if (piece.Colour() == Enums::Colour::White) {
        bb |= GenPawn(piece,  (i64)GRID_SIZE, 1, m_generatingAttacks);
    }
    
    if (piece.Colour() == Enums::Colour::Black) {
        bb |= GenPawn(piece, -(i64)GRID_SIZE, 6, m_generatingAttacks);
    }

    return bb;
}



static BitBoard SelectGeneration(const Piece& piece)
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

static bool CheckForChecks(BitBoard attacks)
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

BitBoard MoveGen::Generate(const Piece& piece)
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

