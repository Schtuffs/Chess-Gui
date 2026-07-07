#include "Board.h"

#include <print>

#include "Fen.h"
#include "MoveGen.h"
#include "Utils.h"



// ----- Creation ----- Destruction -----

Board::Board(std::string_view fen)
    : m_fen(fen), m_castling(0), m_enPassant(UINT8_MAX), m_playerColour(Enums::Colour::White)
{
    if (!Fen::IsValidFen(m_fen.c_str())) {
        ErrorPrintln("Invalid fen: {}", m_fen);
        m_fen = DEFAULT_FEN;
    }
    DebugPrintln("Fen: {}", m_fen);
    
    u8 index = 0;
    for (u64 rank = GRID_SIZE - 1; rank < GRID_SIZE; rank--) {
        for (u64 file = 0; file < GRID_SIZE; file++) {
            u64 i = rank * GRID_SIZE + file;
            char c = fen[index++];

            if (isdigit(c)) {
                file += c - '0' - 1;
                continue;
            }
            
            if (c == '/') {
                rank++;
                break;
            }

            Enums::Colour colour;
            if (isupper(c)) {
                colour = Enums::Colour::White;
            } else {
                colour = Enums::Colour::Black;
            }
            c = tolower(c);

            switch(c) {
            case 'b':
                m_pieces[i] = Piece(colour, Enums::Type::Bishop, i);
                break;
            case 'k':
                m_pieces[i] = Piece(colour, Enums::Type::King,   i);
                break;
            case 'n':
                m_pieces[i] = Piece(colour, Enums::Type::Knight, i);
                break;
            case 'q':
                m_pieces[i] = Piece(colour, Enums::Type::Queen,  i);
                break;
            case 'p':
                m_pieces[i] = Piece(colour, Enums::Type::Pawn,   i);
                break;
            case 'r':
                m_pieces[i] = Piece(colour, Enums::Type::Rook,   i);
                break;
            default:
                m_pieces[i] = Piece();
                break;
            }
        }
    }

    m_playerColour = (fen[index + 1] == 'w' ? Enums::Colour::White : Enums::Colour::Black);
    std::string_view castling = fen.substr(index + 3);
    index = 0;
    char c;
    while ((c = castling[index++]) != ' ') {
        switch(c) {
        case 'K':
            m_castling |= (u8)Enums::Castling::White_King;
            break;
        case 'Q':
            m_castling |= (u8)Enums::Castling::White_Queen;
            break;
        case 'k':
            m_castling |= (u8)Enums::Castling::Black_King;
            break;
        case 'q':
            m_castling |= (u8)Enums::Castling::Black_Queen;
            break;
        }
    }
}

Board::~Board()
{
    // Nothing todo
}



// ----- Read -----

u8 Board::Castling(Enums::Colour colour) const noexcept
{
    if (colour == Enums::Colour::White) {
        return m_castling & ((u8)Enums::Castling::White_King | (u8)Enums::Castling::White_Queen);
    }
    else if (colour == Enums::Colour::Black) {
        return m_castling & ((u8)Enums::Castling::Black_King | (u8)Enums::Castling::Black_Queen);
    }
    else {
        return 0;
    }
}

std::string_view Board::Fen() const noexcept
{
    return m_fen;
}

const Piece* Board::Pieces() const noexcept
{
    return m_pieces;
}

Enums::Colour Board::Player() const noexcept
{
    return m_playerColour;
}



// ----- Update -----

bool Board::MakeMove(std::string_view move)
{
    u8 startPos = Fen::MoveToIndex(move);
    u8 endPos = Fen::MoveToIndex(move.substr(2));

    if (startPos == endPos) {
        return false;
    }

    u64 spots = MoveGen::Generate(*this, m_pieces[startPos]);
    if (!spots) {
        WarningPrintln("Board::MakeMove: Invalid piece at startpos");
        return false;
    }

    u64 index = (u64)1 << endPos;
    if ((index & spots) == 0) {
        return false;
    }

    // Remove old en passant
    if (m_enPassant != UINT8_MAX) {
        m_pieces[m_enPassant] = Piece();
        m_enPassant = UINT8_MAX;
    }

    // Check new en passant
    Piece& p = m_pieces[startPos];
    const Piece& other = m_pieces[endPos];
    bool captureOrPawn = other.IsValid();
    u8 enPassant = UINT8_MAX;
    if (p.Type() == Enums::Type::Pawn) {
        if (p.Colour() == Enums::Colour::White) {
            if (startPos + (2 * GRID_SIZE) == endPos) {
                enPassant = startPos + GRID_SIZE;
                m_pieces[enPassant] = Piece(Enums::Colour::Invalid, Enums::Type::Invalid, enPassant);
            }
        }
        else if (p.Colour() == Enums::Colour::Black) {
            if (startPos - (2 * GRID_SIZE) == endPos) {
                enPassant = startPos - GRID_SIZE;
                m_pieces[enPassant] = Piece(Enums::Colour::Invalid, Enums::Type::Invalid, enPassant);
            }
        }
    }

    if (p.Type() == Enums::Type::King) {
        if (startPos + 2 == endPos) {
            Piece& rook = m_pieces[startPos + 3];
            rook.Position(endPos - 1);
            m_pieces[endPos - 1] = rook;
            m_pieces[startPos + 3] = Piece();
        }
        else if (startPos - 2 == endPos) {
            Piece& rook = m_pieces[startPos - 4];
            rook.Position(endPos + 1);
            m_pieces[endPos + 1] = rook;
            m_pieces[startPos - 4] = Piece();
        }
    }

    p.Position(endPos);
    m_pieces[endPos] = p;
    m_pieces[startPos] = Piece();

    m_playerColour = (
        m_playerColour == Enums::Colour::White ? Enums::Colour::Black : Enums::Colour::White
    );

    RecalculateCastling();
    RecalculateFen(captureOrPawn, enPassant);
    
    return true;
}

// ----- Hidden -----

void Board::RecalculateCastling()
{
    if (m_pieces[4].Type() != Enums::Type::King) {
        m_castling &= ~((u8)Enums::Castling::White_King | (u8)Enums::Castling::White_Queen);
    }
    if (m_pieces[60].Type() != Enums::Type::King) {
        m_castling &= ~((u8)Enums::Castling::Black_King | (u8)Enums::Castling::Black_Queen);
    }
    
    if (m_pieces[0].Type() != Enums::Type::Rook) {
        m_castling &= ~((u8)Enums::Castling::White_Queen);
    }
    if (m_pieces[7].Type() != Enums::Type::Rook) {
        m_castling &= ~((u8)Enums::Castling::White_King);
    }
    if (m_pieces[56].Type() != Enums::Type::Rook) {
        m_castling &= ~((u8)Enums::Castling::Black_Queen);
    }
    if (m_pieces[63].Type() != Enums::Type::Rook) {
        m_castling &= ~((u8)Enums::Castling::Black_King);
    }
}

static std::string CalculateFenPiece(Piece* pieces)
{
    std::string fen;
    u8 extra = 0;
    for (u64 rank = GRID_SIZE - 1; rank < GRID_SIZE; rank--) {
        if (extra > 0) {
            fen += std::to_string(extra);
            extra = 0;
        }

        if (rank != GRID_SIZE - 1) {
            fen += "/";
        }

        for (u64 file = 0; file < GRID_SIZE; file++) {
            u64 i = rank * GRID_SIZE + file;
            
            Piece& piece = pieces[i];
            char p = piece.AsChar();
            if (!p) {
                extra++;
                continue;
            }

            if (extra > 0) {
                fen += std::to_string(extra);
                extra = 0;
            }

            fen += p;
        }
    }

    return fen;
}

void Board::RecalculateFen(bool isCaptureOrPawn, u8 index)
{
    std::string fen = CalculateFenPiece(m_pieces);

    // Player to move
    fen += ' ';
    fen += (m_playerColour == Enums::Colour::White ? 'w' : 'b');

    // Castling rights
    fen += ' ';
    bool hasCastling = false;
    if (m_castling & (u8)Enums::Castling::White_King) {
        fen += 'K';
        hasCastling = true;
    }
    if (m_castling & (u8)Enums::Castling::White_Queen) {
        fen += 'Q';
        hasCastling = true;
    }
    if (m_castling & (u8)Enums::Castling::Black_King) {
        fen += 'k';
        hasCastling = true;
    }
    if (m_castling & (u8)Enums::Castling::Black_Queen) {
        fen += 'q';
        hasCastling = true;
    }

    if (!hasCastling) {
        fen += '-';
    }

    // En passant
    fen += ' ';
    if (index != UINT8_MAX) {
        m_enPassant = index;
        fen += Fen::IndexToMove(m_enPassant);
    }
    else {
        fen += '-';
    }

    // Half moves
    fen += ' ';
    std::string_view strHalfMoves = m_fen.substr(m_fen.find(' ') + 1);
    strHalfMoves = strHalfMoves.substr(strHalfMoves.find(' ') + 1);
    strHalfMoves = strHalfMoves.substr(strHalfMoves.find(' ') + 1);
    strHalfMoves = strHalfMoves.substr(strHalfMoves.find(' ') + 1);
    u32 halfMoves = std::stoul(strHalfMoves.data());
    halfMoves = (isCaptureOrPawn ? 0 : halfMoves + 1);
    fen += std::to_string(halfMoves);
    
    // Whole moves
    fen += ' ';
    strHalfMoves = strHalfMoves.substr(strHalfMoves.find(' ') + 1);
    u32 moves = std::stoul(strHalfMoves.data());
    moves++;
    fen += std::to_string(moves);

    m_fen = fen;
    DebugPrintln("{}", m_fen);
}

