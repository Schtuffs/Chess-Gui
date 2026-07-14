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

enum PinDirection {
    PIN_HORZ        = 1,
    PIN_VERT        = 2,
    PIN_DIAG_UP     = 4,
    PIN_DIAG_DOWN   = 8,
    PIN_NONE        = 16,
};



// ----- Creation / Destruction -----

MoveGen::MoveGen()
  : m_pieceList(nullptr), m_pieceIndex(INVALID),
    m_generatingAttacks(false), m_inCheck(false), m_inDoubleCheck(false), m_pinningPiece(false),
    m_pinIndex(INVALID),
    m_attacks(0), m_pins(0), m_pinsHorz(0), m_pinsVert(0), m_pinsDiagUp(0), m_pinsDiagDown(0)
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

    m_attacks       = 0;
    m_pins          = 0;
    m_pinsHorz      = 0;
    m_pinsVert      = 0;
    m_pinsDiagUp    = 0;
    m_pinsDiagDown  = 0;
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



void MoveGen::UpdatePin(int pinDir)
{
    if (pinDir & PIN_HORZ) {
        m_pinsHorz |= Convert::IndexToBitBoard(m_pinIndex);
        m_pins |= m_pinsHorz;
    }
    
    if (pinDir & PIN_VERT) {
        m_pinsVert |= Convert::IndexToBitBoard(m_pinIndex);
        m_pins |= m_pinsVert;
    }
    
    if (pinDir & PIN_DIAG_UP) {
        m_pinsDiagUp |= Convert::IndexToBitBoard(m_pinIndex);
        m_pins |= m_pinsDiagUp;
    }
    
    if (pinDir & PIN_DIAG_DOWN) {
        m_pinsDiagDown |= Convert::IndexToBitBoard(m_pinIndex);
        m_pins |= m_pinsDiagDown;
    }

    DebugPrintln("PINS: {}", Convert::BitBoardToString(m_pins));
}

int MoveGen::CheckPin(const Piece& other, int pinDir)
{
    if (!m_generatingAttacks) {
        return MOVE_END;
    }

    if (other.Type() == Enums::Type::King) {
        if (m_pinningPiece) {
            UpdatePin(pinDir);
            return MOVE_END;
        }

        if (m_inCheck) {
            m_inDoubleCheck = true;
        }
        m_inCheck = true;

        return MOVE_CAPTURE_KING;
    }

    return MOVE_UNTIL_NEXT;
}

static int CalculatePinDir(Index lhs, Index rhs)
{
    Index lFile = lhs % GRID_SIZE;
    Index lRank = lhs / GRID_SIZE;
    Index rFile = rhs % GRID_SIZE;
    Index rRank = rhs / GRID_SIZE;

    if (lRank == rRank) { return PIN_HORZ; }
    if (lFile == rFile) { return PIN_VERT; }

    int rise = rRank - lRank;
    int run  = rFile - lFile;
    
    if (rise != run) { return PIN_NONE; }

    if (rise > 0) { return PIN_DIAG_UP; }
    if (rise < 0) { return PIN_DIAG_DOWN; }

    return PIN_NONE;
}

int MoveGen::AddMove(const Piece& piece, const Piece& other, Index index, BitBoard& bb)
{
    int compare = PieceCompare(piece, other);

    // Invalid
    if (compare < 0) {
        if (!m_pinningPiece) {
            bb |= Convert::IndexToBitBoard(index);
        }
        return MOVE_CONTINUE;
    }
    // Equal
    else if (compare == 0) {
        if (m_generatingAttacks) {
            bb |= Convert::IndexToBitBoard(index);
        }
        return MOVE_END;
    }
    // Opposite
    else {
        if (m_generatingAttacks) {
            int dir = CalculatePinDir(piece.Position(), other.Position());
            if (CheckPin(other, dir) == MOVE_END) {
                return MOVE_END;
            }
        }
        bb |= Convert::IndexToBitBoard(index);

        return MOVE_END;
    }
}

int MoveGen::AddPawnMove(const Piece& piece, const Piece& other, Index index, BitBoard& bb)
{
    Index pFile = piece.Position() % GRID_SIZE;
    Index oFile = index % GRID_SIZE;
    bool equalFile = pFile == oFile;
    
    // Only get attacks
    if (m_generatingAttacks && equalFile) {
        return MOVE_END;
    }
    
    // Move forward
    if (!other.IsValid() && equalFile) {
        bb |= Convert::IndexToBitBoard(index);
        return MOVE_CONTINUE;
    }
    
    // Attacks
    if (((other.IsValid() || other.IsEnPassant()) &&
        !equalFile && PieceCompare(piece, other) > 0) ||
        m_generatingAttacks
    ) {
        // Ensure left and right file check
        if (std::abs(pFile - oFile) == 1) {
            bb |= Convert::IndexToBitBoard(index);
            return MOVE_CONTINUE;
        }
        return MOVE_CONTINUE;
    }

    return MOVE_END;
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
    Index pos = piece.Position();
    int moves[8] = {-17, -10, 6, 15, 17, 10, -6, -15};

    BitBoard bb = 0;
    for (int i = 0; i < 8; i++) {
        Index index = moves[i] + pos;
        if (index >= GRID_SIZE * GRID_SIZE) {
            continue;
        }

        const Piece& other = m_pieceList[index];
        AddMove(piece, other, index, bb);
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
    if (AddPawnMove(piece, other, checkIndex, bb) == MOVE_CONTINUE) {
        if ((pos / GRID_SIZE) == rank) {
            checkIndex += offset;
            AddPawnMove(piece, other, checkIndex, bb);
        }
    }

    // Left attack
    checkIndex = pos + (offset - 1);
    AddPawnMove(piece, m_pieceList[checkIndex], checkIndex, bb);
    
    // Right attack
    checkIndex = pos + (offset + 1);
    AddPawnMove(piece, m_pieceList[checkIndex], checkIndex, bb);

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

