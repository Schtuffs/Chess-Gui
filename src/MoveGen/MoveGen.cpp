#include "MoveGen/MoveGen.h"

#include <bit>

#include "MoveGen/Magic.h"

#include "Convert.h"
#include "Utils.h"

// ----- Creation ----- Destruction -----

MoveGen::MoveGen() : m_hasGenerated(false) { Magic::GetKingAttacks(1, 2, true); }

// ----- Read -----

BitBoard MoveGen::GetMoves(Index index) const noexcept
{
    if (!Utils::IsValidIndex(index)) {
        return MoveGen::INVALID;
    }

    return m_legal[index];
}

bool MoveGen::IsCheckmate() const noexcept { return m_isCheckmate; }

bool MoveGen::IsStalemate() const noexcept { return m_isStalemate; }

// ----- Update -----

void MoveGen::Generate(const Board& board, Enums::Colour colour)
{
    // Reset state
    Reset();
    m_board     = &board;
    m_genColour = colour;

    // Validate input
    if (colour != Enums::Colour::Black && colour != Enums::Colour::White) {
        return;
    }

    // Prepare bitboards
    SetupPieceBoards();

    // Generate moves
    GenAttacks();
    GenPseudoLegal();
    GenLegal();
}

// ----- Update ----- Hidden -----

// ----- Pre computation -----

void MoveGen::Reset()
{
    // Parameters
    m_hasGenerated = false;
    m_genColour    = Enums::Colour::Invalid;

    // Calculation items
    m_pseudoLegal.fill(MoveGen::INVALID);
    m_friendly = 0;
    m_enemies  = 0;
    m_occupied = 0;

    m_bishops = 0;
    m_kings   = 0;
    m_knights = 0;
    m_pawns   = 0;
    m_queens  = 0;
    m_rooks   = 0;

    m_attacks       = 0;
    m_kingAttacks   = 0;
    m_inCheck       = false;
    m_inDoubleCheck = false;

    // Output items
    m_legal.fill(MoveGen::INVALID);
    m_isCheckmate = false;
    m_isStalemate = false;
}

void MoveGen::SetupPieceBoards()
{
    auto pieces = m_board->Pieces();
    for (const auto& piece : pieces) {
        switch (piece.Type()) {
        case Enums::Type::Bishop:
            m_bishops |= Convert::IndexToBitBoard(piece.Position());
            break;
        case Enums::Type::King:
            m_kings |= Convert::IndexToBitBoard(piece.Position());
            break;
        case Enums::Type::Knight:
            m_knights |= Convert::IndexToBitBoard(piece.Position());
            break;
        case Enums::Type::Pawn:
            m_pawns |= Convert::IndexToBitBoard(piece.Position());
            break;
        case Enums::Type::Queen:
            m_queens |= Convert::IndexToBitBoard(piece.Position());
            break;
        case Enums::Type::Rook:
            m_rooks |= Convert::IndexToBitBoard(piece.Position());
            break;
        default:
            break;
        }

        if (piece.Colour() == m_genColour) {
            m_friendly |= Convert::IndexToBitBoard(piece.Position());
        } else {
            m_enemies |= Convert::IndexToBitBoard(piece.Position());
        }
    }
}

// ----- Generation -----

BitBoard MoveGen::GenMoves(const Piece& piece) const noexcept
{
    BitBoard bb = 0;

    if (!m_generatingAttacks && m_inDoubleCheck) {
        return bb;
    }

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
    case Enums::Type::Queen:
        bb |= GenQueen(piece);
        break;
    case Enums::Type::Pawn:
        bb |= GenPawn(piece);
        break;
    case Enums::Type::Rook:
        bb |= GenRook(piece);
        break;
    default:
        bb = MoveGen::INVALID;
    }

    return bb;
}

BitBoard MoveGen::GenBishop(const Piece& piece) const noexcept
{
    BitBoard bb = 0;

    bb |= Magic::GetSlidingAttacks(piece.Position(), m_friendly | m_enemies, false);

    return bb;
}

BitBoard MoveGen::GenCastling(const Piece& piece) const noexcept
{
    Index king     = piece.Position();
    u8    castling = m_board->Castling();

    BitBoard bb = 0;
    if (Convert::IndexToBitBoard(king) & m_attacks) {
        return bb;
    }

    // Kingside
    BitBoard kMv1 = Convert::IndexToBitBoard(king + 1);
    BitBoard kMv2 = Convert::IndexToBitBoard(king + 2);
    BitBoard kMv  = kMv1 | kMv2;

    // Has castling rights
    if (castling & ((u8)Enums::Castling::White_King | (u8)Enums::Castling::Black_King)) {
        // No attacks allowed
        if ((m_attacks & kMv) == 0) {
            // No pieces allowed
            if ((kMv & (m_friendly | m_enemies)) == 0) {
                bb |= Convert::IndexToBitBoard(king + 2);
            }
        }
    }

    // Queenside
    BitBoard qMv1 = Convert::IndexToBitBoard(king - 1);
    BitBoard qMv2 = Convert::IndexToBitBoard(king - 2);
    BitBoard qMv  = qMv1 | qMv2;

    // Has castling rights
    if (castling & ((u8)Enums::Castling::White_Queen | (u8)Enums::Castling::Black_Queen)) {
        // No attacks allowed
        if ((m_attacks & qMv) == 0) {
            // No pieces allowed
            if ((qMv & (m_friendly | m_enemies)) == 0) {
                bb |= Convert::IndexToBitBoard(king - 2);
            }
        }
    }

    return bb;
}

BitBoard MoveGen::GenKing(const Piece& piece) const noexcept
{
    constexpr BitBoard offsets = 0x00'00'00'00'00'07'05'07;
    constexpr Index    start   = 9;
    BitBoard           bb      = 0;

    // Main moves
    i8 index = (i8)piece.Position() - (i8)start;
    if (index > 0) {
        bb |= offsets << index;
    } else {
        bb |= offsets >> std::abs(index);
    }

    // Prevent wrapping
    Index file = piece.Position() % 8;
    if (file == 0) {
        bb &= ~(0xc0'c0'c0'c0'c0'c0'c0'c0);
    } else if (file == 7) {
        bb &= ~(0x03'03'03'03'03'03'03'03);
    }

    // No need for attack castles
    if (m_generatingAttacks) {
        return bb;
    }

    // Castling
    bb |= GenCastling(piece);

    bb &= ~(m_attacks);
    return bb;
}

BitBoard MoveGen::GenKnight(const Piece& piece) const noexcept
{
    constexpr BitBoard offsets = 0x00'00'00'0a'11'00'11'0a;
    constexpr Index    start   = 18;
    BitBoard           bb      = 0;

    i8 index = (i8)piece.Position() - (i8)start;
    if (index > 0) {
        bb |= offsets << index;
    } else {
        bb |= offsets >> std::abs(index);
    }

    Index file = piece.Position() % 8;
    if (file < 2) {
        bb &= ~(0xc0'c0'c0'c0'c0'c0'c0'c0);
    } else if (file > 5) {
        bb &= ~(0x03'03'03'03'03'03'03'03);
    }

    return bb;
}

BitBoard MoveGen::GenQueen(const Piece& piece) const noexcept
{
    BitBoard bb = 0;

    bb |= GenBishop(piece);
    bb |= GenRook(piece);

    return bb;
}

BitBoard MoveGen::GenPawn(const Piece& piece) const noexcept
{
    BitBoard bb  = 0;
    BitBoard m1  = 0;
    BitBoard m2  = 0;
    BitBoard pos = Convert::IndexToBitBoard(piece.Position());

    // Moves
    if (piece.Colour() == Enums::Colour::White) {
        m1 |= pos << 8;
        if ((piece.Position() / 8) == 1) {
            m2 |= pos << 16;
        }
    } else {
        m1 |= pos >> 8;
        if ((piece.Position() / 8) == 6) {
            m2 |= pos >> 16;
        }
    }

    // Gen atttacks first
    if (m_generatingAttacks) {
        if ((m1 >> 1) & m_friendly) {
            bb |= m1 >> 1;
        }

        if ((m1 << 1) & m_friendly) {
            bb |= m1 << 1;
        }

        return bb;
    }

    if ((m1 >> 1) & m_enemies) {
        bb |= (m1 >> 1);
    }

    if ((m1 << 1) & m_enemies) {
        bb |= (m1 << 1);
    }

    // Cant move into pieces
    if (!(m1 & (m_friendly | m_enemies))) {
        bb |= m1;
        if (!(m2 & (m_friendly | m_enemies))) {
            bb |= m2;
        }
    }

    return bb;
}

BitBoard MoveGen::GenRook(const Piece& piece) const noexcept
{
    BitBoard bb = 0;

    bb |= Magic::GetSlidingAttacks(piece.Position(), m_friendly | m_enemies, true);

    return bb;
}

// ----- Attacks -----

void MoveGen::AddAttacks(const Piece& piece, const Piece& king, BitBoard moves)
{
    switch (piece.Type()) {
    case Enums::Type::Bishop:
        m_kingAttacks |= Magic::GetKingAttacks(piece.Position(), king.Position(), false);
        break;
    case Enums::Type::King:
        // What?!
        break;
    case Enums::Type::Knight:
        m_kingAttacks |= Convert::IndexToBitBoard(piece.Position());
        break;
    case Enums::Type::Queen:
        m_kingAttacks |= Magic::GetKingAttacks(piece.Position(), king.Position(), false);
        m_kingAttacks |= Magic::GetKingAttacks(piece.Position(), king.Position(), true);
        break;
    case Enums::Type::Pawn:
        m_kingAttacks |= Convert::IndexToBitBoard(piece.Position());
        break;
    case Enums::Type::Rook:
        m_kingAttacks |= Magic::GetKingAttacks(piece.Position(), king.Position(), true);
        break;
    default:
        m_kingAttacks |= moves;
        break;
    }
}

void MoveGen::AddCheck()
{
    if (m_inCheck) {
        m_inDoubleCheck = true;
    }
    m_inCheck = true;
}

// ----- Main generation loops -----

void MoveGen::GenAttacks()
{
    m_generatingAttacks = true;

    auto          pieces = m_board->Pieces();
    Enums::Colour enemy  = Utils::SwapColour(m_genColour);

    // Find king
    const Piece* king = nullptr;
    for (Index i = 0; i < 64; i++) {
        const Piece& piece = pieces[i];
        if (piece.Type() != Enums::Type::King) {
            continue;
        }

        if (piece.Colour() != m_genColour) {
            continue;
        }

        king = &piece;
        break;
    }

    // Gen attacks and pins
    for (Index i = 0; i < 64; i++) {
        const Piece& piece = pieces[i];
        if (piece.Colour() != enemy) {
            continue;
        }

        BitBoard moves = GenMoves(piece);
        // If king, check if attack
        if (king) {
            BitBoard kingPos = Convert::IndexToBitBoard(king->Position());
            // Add to pin attacks
            if (moves & kingPos) {
                AddAttacks(piece, *king, moves);
                AddCheck();
            }
        }

        m_attacks |= moves;
    }

    m_generatingAttacks = false;
}

void MoveGen::GenPseudoLegal()
{
    auto pieces = m_board->Pieces();

    for (Index i = 0; i < 64; i++) {
        const Piece& piece = pieces[i];
        if (piece.Colour() != m_genColour) {
            continue;
        }

        // Get moves and remove the friendly squares
        BitBoard moves = GenMoves(piece);
        moves &= ~(m_friendly);
        m_pseudoLegal[i] = moves;
    }
}

void MoveGen::GenLegal()
{
    // Play move, check state, unplay move
    for (Index i = 0; i < 64; i++) {
        const Piece& piece = m_board->Pieces()[i];
        BitBoard     bb    = m_pseudoLegal[i];

        if (m_inCheck && piece.Type() != Enums::Type::King) {
            bb &= m_kingAttacks;
        }

        if (piece.IsValid()) {
            bb |= Convert::IndexToBitBoard(piece.Position());
            m_legal[i] = bb;
        }
    }
}
