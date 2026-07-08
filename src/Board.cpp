#include "Board.h"

#include <print>

#include "Convert.h"
#include "Fen.h"
#include "MoveGen.h"
#include "Utils.h"

constexpr Index INVALID_ENPASSANT   = UINT8_MAX;



// ----- Creation ----- Destruction -----

Board::Board(std::string_view fen)
    : m_fen(fen), m_castling(0), m_enPassant(INVALID_ENPASSANT), m_playerColour(Enums::Colour::White)
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
            char c = m_fen[index++];

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

    m_playerColour = (m_fen[index + 1] == 'w' ? Enums::Colour::White : Enums::Colour::Black);
    std::string_view castling = m_fen.substr(index + 3);
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

    std::string_view enPassant = castling.substr(index);
    if (enPassant[0] != '-') {
        m_enPassant = Convert::MoveToIndex(enPassant);
        m_pieces[m_enPassant] = Piece(m_enPassant);
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
    Index startPos = Convert::MoveToIndex(move);
    Index endPos = Convert::MoveToIndex(move.substr(2));

    if (startPos >= GRID_SIZE * GRID_SIZE || endPos >= GRID_SIZE * GRID_SIZE) {
        ErrorPrintln("Board::MakeMove: Invalid start or end pos: {}, {}", startPos, endPos);
        return false;
    }

    if (startPos == endPos) {
        InfoPrintln("Board::MakeMove: Piece returned to starting position.");
        return false;
    }

    BitBoard movesBB = MoveGen::Generate(*this, m_pieces[startPos]);
    if (!movesBB) {
        WarningPrintln("Board::MakeMove: Invalid piece at startpos.");
        return false;
    }

    BitBoard indexBB = Convert::IndexToBitBoard(endPos);
    if ((indexBB & movesBB) == 0) {
        InfoPrintln("Board::MakeMove: End position was invalid.");
        return false;
    }

    m_playerColour = (
        m_playerColour == Enums::Colour::White ? Enums::Colour::Black : Enums::Colour::White
    );

    Piece& piece = m_pieces[startPos];
    Piece other = MovePiece(startPos, endPos);
    RecalculateEnPassant(startPos, endPos);
    RecalculateCastling(startPos, endPos);
    RecalculateFen((piece.Type() == Enums::Type::Pawn) | (other.IsValid()));
    
    return true;
}

// ----- Hidden -----

Piece Board::MovePiece(Index start, Index end)
{
    Piece p = m_pieces[start];
    p.Position(end);
    Piece toRet = m_pieces[end];
    m_pieces[end] = p;
    m_pieces[start] = Piece();
    return toRet;
}

void Board::RecalculateCastling(Index start, Index end)
{
    Piece& piece = m_pieces[start];
    if (piece.Type() == Enums::Type::King) {
        // Short castle
        if (start + 2 == end) {
            Piece& rook = m_pieces[start + 3];
            rook.Position(end - 1);
            m_pieces[end - 1] = rook;
            m_pieces[start + 3] = Piece();
        }
        // Long castle
        else if (start - 2 == end) {
            Piece& rook = m_pieces[start - 4];
            rook.Position(end + 1);
            m_pieces[end + 1] = rook;
            m_pieces[start - 4] = Piece();
        }
    }

    // Remove all castling if king moved
    if (m_pieces[4].Type() != Enums::Type::King) {
        m_castling &= ~((u8)Enums::Castling::White_King | (u8)Enums::Castling::White_Queen);
    }
    if (m_pieces[60].Type() != Enums::Type::King) {
        m_castling &= ~((u8)Enums::Castling::Black_King | (u8)Enums::Castling::Black_Queen);
    }
    
    // Individual castling checks
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

void Board::RecalculateEnPassant(Index start, Index end)
{
    // Remove old en passant
    if (m_enPassant != INVALID_ENPASSANT) {
        if (!m_pieces[m_enPassant].IsValid()) {
            m_pieces[m_enPassant] = Piece();
        }
        m_enPassant = INVALID_ENPASSANT;
    }

    // Check new en passant
    Piece& piece = m_pieces[end];
    if (piece.Type() != Enums::Type::Pawn) {
        return;
    }

    if (piece.Colour() == Enums::Colour::White) {
        if (start + (2 * GRID_SIZE) == end) {
            m_enPassant = start + GRID_SIZE;
            m_pieces[m_enPassant] = Piece(m_enPassant);
        }
    }
    else if (piece.Colour() == Enums::Colour::Black) {
        if (start - (2 * GRID_SIZE) == end) {
            m_enPassant = start - GRID_SIZE;
            m_pieces[m_enPassant] = Piece(m_enPassant);
        }
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

void Board::RecalculateFen(bool isCaptureOrPawn)
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
    if (m_enPassant != INVALID_ENPASSANT) {
        fen += Convert::IndexToMove(m_enPassant);
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
    if (m_playerColour == Enums::Colour::White) {
        moves++;
    }
    fen += std::to_string(moves);

    m_fen = fen;
    if (!Fen::IsValidFen(m_fen.c_str())) {
        ErrorPrintln("Board::RecalculateFen: Produced an invalid fen!");
    }
    
    DebugPrintln("{}", m_fen);
}

