#include "MoveGen/MoveGen.h"

#include "MoveGen/Magic.h"

#include "Convert.h"
#include "Utils.h"

// ----- Creation ----- Destruction -----

MoveGen::MoveGen() : m_hasGenerated(false) {}

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
    m_board     = board;
    m_genColour = colour;

    // Validate input
    if (colour != Enums::Colour::Black && colour != Enums::Colour::White) {
        return;
    }

    // Prepare bitboards
    SetupPieceBoards();

    // Generate moves
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

    // Output items
    m_legal.fill(MoveGen::INVALID);
    m_isCheckmate = false;
    m_isStalemate = false;
}

void MoveGen::SetupPieceBoards()
{
    auto pieces = m_board.Pieces();
    for (const auto& piece : pieces) {
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

    switch (piece.Type()) {
    case Enums::Type::Bishop:
        bb |= GenBishop(piece);
        break;
    case Enums::Type::King:
        // bb |= GenKing(piece);
        break;
    case Enums::Type::Knight:
        // bb |= GenKnight(piece);
        break;
    case Enums::Type::Queen:
        bb |= GenBishop(piece);
        bb |= GenRook(piece);
        break;
    case Enums::Type::Pawn:
        // bb |= GenPawn(piece);
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

BitBoard MoveGen::GenKing(const Piece& piece) const noexcept
{
    (void)piece;
    BitBoard bb = 0;

    return bb;
}

BitBoard MoveGen::GenRook(const Piece& piece) const noexcept
{
    BitBoard bb = 0;

    bb |= Magic::GetSlidingAttacks(piece.Position(), m_friendly | m_enemies, true);

    return bb;
}

// ----- Main generation loops -----

void MoveGen::GenPseudoLegal()
{
    auto pieces = m_board.Pieces();

    for (Index i = 0; i < 64; i++) {
        const Piece& piece = pieces[i];
        if (piece.Colour() != m_genColour) {
            continue;
        }

        BitBoard moves = GenMoves(piece);
        moves &= ~(m_friendly);
        moves |= Convert::IndexToBitBoard(piece.Position());
        m_pseudoLegal[i] = moves;
    }
}

void MoveGen::GenLegal()
{
    // Prepare data
    // Enums::Colour attacker = Utils::SwapColour(m_genColour);

    // Play move, check state, unplay move
    for (Index i = 0; i < 64; i++) {
        m_legal[i] = m_pseudoLegal[i];
        continue;
        const Piece& piece = m_board.Pieces()[i];

        if (piece.Colour() != m_genColour) {
            continue;
        }

        BitBoard bb = IterateMoves(piece, m_pseudoLegal[i]);
        m_legal[i]  = bb;
    }
}

BitBoard MoveGen::IterateMoves(const Piece& piece, BitBoard moves)
{
    MoveGen     gen;
    BitBoard    bb    = 0;
    std::string start = Convert::IndexToMove(piece.Position());
    moves &= ~(Convert::IndexToBitBoard(piece.Position()));

    for (Index i = 0; i < 64; i++) {
        if (!(Convert::IndexToBitBoard(i) & moves)) {
            continue;
        }

        // Make move
        std::string end  = Convert::IndexToMove(i);
        std::string move = start + end;
        if (!m_board.MakeMove(move)) {
            continue;
        }

        // Unmake move
        m_board.UnmakeMove();

        bb |= 1;
    }

    return bb;
}
