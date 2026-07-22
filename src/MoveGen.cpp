#include "MoveGen.h"

#include <cmath>
#include <print>

#include "Convert.h"
#include "Utils.h"

enum MoveResult {
    MOVE_CONTINUE = 1,
    MOVE_UNTIL_NEXT,
    MOVE_END,
    MOVE_PIN_TO_KING,
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
  : m_pieceList(nullptr), m_pieceIndex(INVALID), m_castling(0),
    m_generatingAttacks(false), m_inCheck(false), m_inDoubleCheck(false), m_pinningPiece(false),
    m_pinIndex(INVALID),
    m_attacks(0), m_pins(0), m_pinsHorz(0), m_pinsVert(0), m_pinsDiagUp(0), m_pinsDiagDown(0),
    m_checkSquares(0), m_currentMoves(0)
{}



// ----- Read ----- Hidden -----

bool MoveGen::IsSquareAttacked(Index index)
{
    return (m_attacks & (Convert::IndexToBitBoard(index)));
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



// ----- Update -----

BitBoard MoveGen::Generate(const Piece* pieces, Index index, u8 castling)
{
    // Check pieces
    if (pieces == nullptr) {
        WarningPrintln("MoveGen::Generate: Piece list was null.");
        return INVALID;
    }

    // Verify index
    if (!Utils::IsValidIndex(index)) {
        WarningPrintln("MoveGen::Generate: Index out of bounds: {}", index);
        return INVALID;
    }

    // Ensure castling only for one side
    if (
        (castling & ((u8)Enums::Castling::Black_King | (u8)Enums::Castling::Black_Queen)) > 0 &&
        (castling & ((u8)Enums::Castling::White_King | (u8)Enums::Castling::White_Queen)) > 0
    ) {
        WarningPrintln("MoveGen::Generate: Castling should only be for one player: {}", castling);
        return INVALID;
    }

    // Reset and prepare for new move generation
    Reset();
    m_pieceList = pieces;
    m_pieceIndex = index;
    m_castling = castling;

    // Check piece selected is valid
    const Piece& piece = m_pieceList[m_pieceIndex];
    if (!piece.IsValid()) {
        WarningPrintln("MoveGen::Generate: Invalid piece selected.");
        return MoveGen::INVALID;
    }

    // Always generate attacks first
    m_attacks = GenAttacks();
    DebugPrintln("MoveGen::Generate: Attacks: {}", Convert::BitBoardToString(m_attacks));

    // If in double check, only king can move
    if (m_inDoubleCheck) {
        DebugPrintln("MoveGen::Generate: In double check.");
        if (piece.Type() == Enums::Type::King) {
            return GenMoves(piece);
        }
        return Convert::IndexToBitBoard(piece.Position());
    }

    // Generate moves normally
    BitBoard bb = GenMoves(piece);
    DebugPrintln("MoveGen::Generate: Moves: {}", Convert::BitBoardToString(bb, 'X', ' '));
    return bb;
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

    m_checkSquares = 0;
    m_currentMoves = 0;
}



// ----- Attacks -----

BitBoard MoveGen::GenAttacks()
{
    m_generatingAttacks = true;

    BitBoard bb = 0;
    const Piece* pieces = m_pieceList;
    Enums::Colour currentColour = m_pieceList[m_pieceIndex].Colour();

    for (Index i = 0; i < 64; i++) {
        m_pinningPiece = false;
        m_currentMoves = 0;

        const Piece& piece = pieces[i];
        if (!piece.IsValid() || piece.Colour() == currentColour) {
            continue;
        }

        bb |= GenMoves(piece);
    }

    m_pinningPiece = false;
    m_generatingAttacks = false;

    return bb;
}

void MoveGen::ResetAttackPiece()
{
    m_pinningPiece = false;
    m_pinIndex = 64;
    m_currentMoves = 0;
}



// ----- Checks -----

void MoveGen::AddCheck()
{
    // Update checks
    if (m_inCheck) {
        m_inDoubleCheck = true;
    }
    m_inCheck = true;
    DebugPrintln("MoveGen::AddCheck: Check: {}, Double Check: {}", m_inCheck, m_inDoubleCheck);
}

void MoveGen::AddCheckMoves(const Piece& piece)
{
    m_checkSquares |= Convert::IndexToBitBoard(piece.Position());
    if (piece.Type() != Enums::Type::Knight && piece.Type() != Enums::Type::Pawn) {
        m_checkSquares |= m_currentMoves;
    }
}

bool MoveGen::IsBlockCheck(Index index)
{
    return (!m_generatingAttacks && m_inCheck && !m_inDoubleCheck &&
        (Convert::IndexToBitBoard(index) & m_checkSquares) > 0
    );
}



// ----- Pins -----

void MoveGen::AddPiecePin(int pinDir)
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
}

int MoveGen::IsNewPin(const Piece& piece, const Piece& other, int pinDir)
{
    // Only check pins on attack generation
    if (!m_generatingAttacks) {
        return MOVE_END;
    }

    // Not pinning piece could be direct king attack
    if (!m_pinningPiece) {
        if (other.Type() == Enums::Type::King) {
            AddCheck();
            AddCheckMoves(piece);
            // Allow skewer
            return MOVE_UNTIL_NEXT;
        }

        // Begin pinning this piece
        if (piece.Type() != Enums::Type::Knight) {
            m_pinningPiece = true;
            m_pinIndex = other.Position();
        }
        return MOVE_UNTIL_NEXT;
    }

    // No longer pinning
    m_pinningPiece = false;

    // Check if pin needs to be added
    if (other.Type() == Enums::Type::King) {
        AddPiecePin(pinDir);
        return MOVE_PIN_TO_KING;
    }

    return MOVE_END;
}

int MoveGen::IsPiecePinned(const Piece& piece)
{
    Index index = piece.Position();
    BitBoard pos = Convert::IndexToBitBoard(index);

    if (m_generatingAttacks) {
        return PIN_NONE;
    }

    if (!(pos & m_pins)) {
        return PIN_NONE;
    }

    if (pos & m_pinsHorz) {
        return PIN_HORZ;
    }

    if (pos & m_pinsVert) {
        return PIN_VERT;
    }

    if (pos & m_pinsDiagUp) {
        return PIN_DIAG_UP;
    }

    if (pos & m_pinsDiagDown) {
        return PIN_DIAG_DOWN;
    }

    return PIN_NONE;
}

static int CalculatePinDir(Index lhs, Index rhs)
{
    Index lFile = lhs % 8;
    Index lRank = lhs / 8;
    Index rFile = rhs % 8;
    Index rRank = rhs / 8;

    if (lRank == rRank) { return PIN_HORZ; }
    if (lFile == rFile) { return PIN_VERT; }

    int rise = rRank - lRank;
    int run  = rFile - lFile;

    if (std::abs(rise) != std::abs(run)) { return PIN_NONE; }

    if ((rise / run) > 0) { return PIN_DIAG_UP; }
    if ((rise / run) < 0) { return PIN_DIAG_DOWN; }

    return PIN_NONE;
}



// ----- Verification -----

int MoveGen::AddMove(const Piece& piece, Index index)
{
    // Valid index
    if (!Utils::IsValidIndex(index)) {
        WarningPrintln("MoveGen::AddMove: Index out of bounds: {}", index);
        return MOVE_END;
    }

    // Check for blocking king check with another piece
    const Piece& other = m_pieceList[index];
    if (!m_generatingAttacks) {
        if (piece.Type() != Enums::Type::King && m_inCheck && !IsBlockCheck(index)) {
            // Don't allow moving onto friendly piece
            if (PieceCompare(piece, other) == 0) {
                return MOVE_END;
            }
            return MOVE_CONTINUE;
        }
    }

    // Invalid
    int compare = PieceCompare(piece, other);
    if (compare < 0) {
        if (!m_pinningPiece) {
            m_currentMoves |= Convert::IndexToBitBoard(index);
        }
        return MOVE_CONTINUE;
    }
    // Equal
    else if (compare == 0) {
        if (m_generatingAttacks) {
            m_currentMoves |= Convert::IndexToBitBoard(index);
        }
        return MOVE_END;
    }
    // Opposite
    if (!m_generatingAttacks) {
        m_currentMoves |= Convert::IndexToBitBoard(index);
        return MOVE_END;
    }

    // Pin generation
    int dir = CalculatePinDir(piece.Position(), other.Position());
    int res = IsNewPin(piece, other, dir);
    if (res == MOVE_PIN_TO_KING) {
        return MOVE_PIN_TO_KING;
    }

    m_currentMoves |= Convert::IndexToBitBoard(index);

    return res;
}

int MoveGen::AddPawnMove(const Piece& piece, Index index)
{
    if (m_inCheck && !IsBlockCheck(index)) {
        return MOVE_END;
    }

    Index pFile = piece.Position() % 8;
    Index oFile = index % 8;
    bool equalFile = pFile == oFile;

    // Only get attacks
    if (m_generatingAttacks && equalFile) {
        return MOVE_END;
    }

    // Check for pins
    int pins = IsPiecePinned(piece);
    if (pins != PIN_NONE) {
        if (equalFile && pins != PIN_VERT) {
            return MOVE_END;
        }
    }

    // Attacks
    const Piece& other = m_pieceList[index];
    if (m_generatingAttacks && other.Type() == Enums::Type::King) {
        AddCheck();
        AddCheckMoves(piece);
    }
    else if (other.IsValid() || other.IsEnPassant()) {
        if (!equalFile && PieceCompare(piece, other) != 0) {
            // Check file wraps
            if (std::abs(pFile - oFile) == 1) {
                m_currentMoves |= Convert::IndexToBitBoard(index);
            }
        }
        return MOVE_END;
    }

    // Move forward
    if (equalFile && !other.IsValid()) {
        m_currentMoves |= Convert::IndexToBitBoard(index);
        return MOVE_CONTINUE;
    }

    return MOVE_END;
}



BitBoard MoveGen::GenSliding(const Piece& piece, i32 offset, Index mod)
{
    ResetAttackPiece();

    BitBoard bb = 0;

    bool untilNext = false;
    for (Index i = 1; i < 8; i++) {
        Index index = piece.Position();
        index += (i32)i * offset;
        if (!Utils::IsValidIndex(index)) {
            DebugPrintln("MoveGen::GenSliding: Index out of bounds: {}", index);
            break;
        }

        if ((index % (Index)8) == mod) {
            DebugPrintln("MoveGen::GenSliding: Index is mod");
            break;
        }

        int res = AddMove(piece, index);
        if (res == MOVE_END) {
            break;
        }
        if (res == MOVE_UNTIL_NEXT) {
            untilNext = true;
        }
        else if (res == MOVE_END && untilNext) {
            break;
        }
    }

    bb = m_currentMoves;

    return bb;
}

BitBoard MoveGen::GenBishop(const Piece& piece)
{
    BitBoard bb = 0;

    int pins = IsPiecePinned(piece);
    if (pins & (PIN_DIAG_UP | PIN_NONE) || m_generatingAttacks) {
        bb |= GenSliding(piece,  9, 0); // Up right
        bb |= GenSliding(piece, -9, 7); // Down left
    }
    if (pins & (PIN_DIAG_DOWN | PIN_NONE) || m_generatingAttacks) {
        bb |= GenSliding(piece,  7, 7); // Up left
        bb |= GenSliding(piece, -7, 0); // Down right
    }

    return bb;
}

BitBoard MoveGen::GenRook(const Piece& piece)
{
    BitBoard bb = 0;

    int pins = IsPiecePinned(piece);
    if (pins & (PIN_VERT | PIN_NONE) || m_generatingAttacks) {
        bb |= GenSliding(piece,  8, 0xff); // Up
        bb |= GenSliding(piece, -8, 0xff); // Down
    }
    if (pins & (PIN_HORZ | PIN_NONE) || m_generatingAttacks) {
        bb |= GenSliding(piece,  1,    0); // Right
        bb |= GenSliding(piece, -1,    7); // Left
    }

    return bb;
}

BitBoard MoveGen::GenQueen(const Piece& piece)
{
    BitBoard bb = 0;

    bb |= GenRook(piece);
    bb |= GenBishop(piece);

    return bb;
}



bool MoveGen::IsValidForCastle(Index index)
{
    return (!m_pieceList[index].IsValid() && !IsSquareAttacked(index) && !m_inCheck);
}

BitBoard MoveGen::GenCastling(const Piece& piece)
{
    if (m_castling & ((u8)Enums::Castling::White_King | (u8)Enums::Castling::Black_King)) {
        Index index = piece.Position();
        if (IsValidForCastle(index + 1) && IsValidForCastle(index + 2)) {
            AddMove(piece, piece.Position() + 2);
        }
    }

    if (m_castling & ((u8)Enums::Castling::White_Queen | (u8)Enums::Castling::Black_Queen)) {
        Index index = piece.Position();
        if (IsValidForCastle(index - 1) && IsValidForCastle(index - 2)) {
            AddMove(piece, index - 2);
        }
    }

    BitBoard bb = m_currentMoves;

    return bb;
}

BitBoard MoveGen::GenKing(const Piece& piece)
{
    ResetAttackPiece();

    Index pos = piece.Position();
    BitBoard bb = 0;

    for (int rank = -1; rank < 2; rank++) {
        if (rank == -1 && (pos / 8) == 0) {
            continue;
        }

        if (rank == 1 && (pos / 8) == 8 - 1) {
            continue;
        }

        for (int file = -1; file < 2; file++) {
            if (file == -1 && (pos % 8) == 0) {
                continue;
            }

            if (file == 1 && (pos % 8) == 8 - 1) {
                continue;
            }

            Index index = (Index)((Index)pos + (rank * (Index)8) + file);
            if (m_generatingAttacks) {
                if (!(file == 0 && rank == 0)) {
                    AddMove(piece, index);
                }
                continue;
            }

            // Can't move into attacked square
            if (m_attacks & Convert::IndexToBitBoard(index)) {
                continue;
            }

            AddMove(piece, index);
        }
    }

    if (!m_generatingAttacks) {
        GenCastling(piece);
    }
    bb = m_currentMoves;

    return bb;
}

BitBoard MoveGen::GenKnight(const Piece& piece)
{
    ResetAttackPiece();

    constexpr int HOP_INVALID   = 0x7f;
    Index pos = piece.Position();
    BitBoard bb = 0;
    if (m_pins & Convert::IndexToBitBoard(pos)) {
        return bb;
    }

    int moves[8] = {-17, -10, 6, 15, 17, 10, -6, -15};
    int file = pos % (int)8;

    // Stop left overflow
    if (file <= 1) {
        moves[1] = HOP_INVALID;
        moves[2] = HOP_INVALID;
        if (file == 0) {
            moves[0] = HOP_INVALID;
            moves[3] = HOP_INVALID;
        }
    }

    // Stop right overflow
    if (file >= ((int)8 - 2)) {
        moves[5] = HOP_INVALID;
        moves[6] = HOP_INVALID;
        if (file == ((int)8 - 1)) {
            moves[4] = HOP_INVALID;
            moves[7] = HOP_INVALID;
        }
    }

    // Add attack moves
    for (int i = 0; i < 8; i++) {
        Index index = moves[i] + pos;
        if (!Utils::IsValidIndex(index)) {
            continue;
        }

        AddMove(piece, index);
    }

    bb = m_currentMoves;

    return bb;
}

BitBoard MoveGen::GenPawn(const Piece& piece)
{
    ResetAttackPiece();

    // Allow both types to use same code
    const i8 offset = ((piece.Colour() == Enums::Colour::White) ? (i8)8 : (-(i8)8));
    const Index rank = ((piece.Colour() == Enums::Colour::White) ? 1 : 6);

    // Prepare state
    BitBoard bb = 0;
    Index pos = piece.Position();
    Index checkIndex = pos + offset;

    // Forward moves
    if (AddPawnMove(piece, checkIndex) == MOVE_CONTINUE) {
        if ((pos / 8) == rank) {
            checkIndex += offset;
            AddPawnMove(piece, checkIndex);
        }
    }

    // Left attack
    checkIndex = pos + (offset - 1);
    AddPawnMove(piece, checkIndex);

    // Right attack
    checkIndex = pos + (offset + 1);
    AddPawnMove(piece, checkIndex);

    bb = m_currentMoves;
    return bb;
}



BitBoard MoveGen::GenMoves(const Piece& piece)
{
    ResetAttackPiece();

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

