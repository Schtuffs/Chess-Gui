#include "Board.h"

#include <print>

#include "Convert.h"
#include "Fen.h"
#include "MoveGen.h"
#include "Utils.h"

constexpr Index INVALID_ENPASSANT   = 64;



// ----- Creation ----- Destruction -----

Board::Board(std::string_view fen)
  : m_fen(fen),
    m_castling(0), m_enPassant(INVALID_ENPASSANT),
    m_playerColour(Enums::Colour::White)
{
    if (!Fen::IsValidFen(m_fen.c_str())) {
        ErrorPrintln("Board::Board: Invalid fen: {}", m_fen);
        m_fen = DEFAULT_FEN;
    }
    DebugPrintln("Board::Board: Fen: {}", m_fen);

    u8 index = 0;
    for (u64 rank = 8 - 1; rank < 8; rank--) {
        for (u64 file = 0; file < 8; file++) {
            u64 i = rank * 8 + file;
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

    std::string_view halfMoves = enPassant.substr(enPassant.find(' ') + 1);

    std::string_view fullMoves = halfMoves.substr(halfMoves.find(' ') + 1);
    if (fullMoves.length() == 1 && fullMoves[0] == '1') {
        m_fen[m_fen.length() - 1] = '0';
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
        return (m_castling & ((u8)Enums::Castling::White_King | (u8)Enums::Castling::White_Queen));
    }
    else if (colour == Enums::Colour::Black) {
        return (m_castling & ((u8)Enums::Castling::Black_King | (u8)Enums::Castling::Black_Queen));
    }
    else {
        return 0;
    }
}

std::string_view Board::Fen() const noexcept
{
    return m_fen;
}

std::span<const Piece, 64> Board::Pieces() const noexcept
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
    // Validate input
    move = Convert::CastleToMove(move, m_playerColour);
    if (move.length() < 4) {
        return false;
    }

    Index startPos = Convert::MoveToIndex(move);
    Index endPos = Convert::MoveToIndex(move.substr(2));

    // Attempt to play the move
    if (!ValidateMove(startPos, endPos)) {
        InfoPrintln("Board::MakeMove: Could not play move: {}", move);
        return false;
    }

    // Perform pawn functions
    MoveEnPassant(move);

    // Perform king functions
    MoveCastling(move);

    // Play the move and swap turns
    Piece piece = m_pieces[startPos];
    Piece other = m_pieces[endPos];
    MovePiece(move);
    piece = m_pieces[endPos];
    m_playerColour = (
        m_playerColour == Enums::Colour::White ? Enums::Colour::Black : Enums::Colour::White
    );

    // Update fen data
    m_fen = RecalculateFen((piece.Type() == Enums::Type::Pawn) | (other.IsValid() | other.IsEnPassant()));
    DebugPrintln("Board::MakeMove: Fen: {}", m_fen);

    return true;
}

bool Board::PromotePawn(Index index, Enums::Type type)
{
    if (index >= 64) {
        return false;
    }

    // Piece must be pawn
    Piece& piece = m_pieces[index];
    if (!piece.IsValid() || piece.Type() != Enums::Type::Pawn) {
        return false;
    }

    // Valid promotion type
    if (type != Enums::Type::Bishop &&
        type != Enums::Type::Knight &&
        type != Enums::Type::Queen  &&
        type != Enums::Type::Rook
    ) {
        return false;
    }

    // Add the piece
    InfoPrintln("Board::PromotePawn: Promoting: {} to {}", piece.ToString(), Enums::ToString::Type[(u8)type]);
    piece = Piece(piece.Colour(), type, piece.Position());

    return true;
}

// ----- Hidden -----

bool Board::ValidateMove(Index start, Index end)
{
    // Check indexes
    if (start >= 64 || end >= 64) {
        WarningPrintln("Board::ValidateMove: Invalid start or end pos: {}, {}", start, end);
        return false;
    }

    // Piece returned to start square
    if (start == end) {
        InfoPrintln("Board::ValidateMove: Piece returned to starting position.");
        return false;
    }

    // Check piece validity
    const Piece& piece = m_pieces[start];
    if (piece.Colour() != m_playerColour) {
        WarningPrintln("Board::ValidateMove: Invalid piece selected at start position: {}", piece.ToString());
        return false;
    }

    // Ensure valid move generation
    BitBoard movesBB = m_moveGen.Generate(m_pieces.data(), start, Castling(Player()));
    if (!movesBB) {
        WarningPrintln("Board::ValidateMove: Could not generate moves for piece: {}", piece.ToString());
        return false;
    }

    // Check valid end position selection
    BitBoard indexBB = Convert::IndexToBitBoard(end);
    if ((indexBB & movesBB) == 0) {
        InfoPrintln("Board::ValidateMove: End position was invalid.");
        return false;
    }

    return true;
}

void Board::MovePiece(std::string_view move)
{
    Index start = Convert::MoveToIndex(move);
    Index end = Convert::MoveToIndex(move.substr(2));

    Piece piece = m_pieces[start];

    // Swap pieces
    piece.Position(end);
    m_pieces[end] = piece;
    m_pieces[start] = Piece();
}

void Board::MoveEnPassant(std::string_view move)
{
    // Reset en passant
    Index enPassant = m_enPassant;
    m_enPassant = INVALID_ENPASSANT;

    // Calculate information
    Index start = Convert::MoveToIndex(move);
    Index end = Convert::MoveToIndex(move.substr(2));
    Piece& piece = m_pieces[start];
    Piece& other = m_pieces[end];

    // Ensure piece is pawn
    if (piece.Type() != Enums::Type::Pawn) {
        return;
    }

    // Update en passant square (if necessary)
    i8 offset = (piece.Colour() == Enums::Colour::White ? (i8)8 : -(i8)8);
    if (start + (offset * 2) == end) {
        m_enPassant = start + offset;
        m_pieces[m_enPassant] = Piece(m_enPassant);
    }

    // Must be taking en passant square
    if (other.IsEnPassant() && end == enPassant) {
        // Eliminate the peasant
        m_pieces[enPassant - offset] = Piece();
    }
}

void Board::MoveCastling(std::string_view move)
{
    if (move.length() < 4) {
        ErrorPrintln("Board::MoveCastling: move ({}) was too short: {}", move, move.length());
        return;
    }

    Index start = Convert::MoveToIndex(move);
    Index end = Convert::MoveToIndex(move.substr(2));

    Piece& piece = m_pieces[start];
    if (piece.Type() == Enums::Type::King) {
        // Short castle
        if (start + 2 == end) {
            Piece& rook = m_pieces[end + 1];
            rook.Position(end - 1);
            m_pieces[end - 1] = rook;
            m_pieces[end + 1] = Piece();
        }
        // Long castle
        else if (start - 2 == end) {
            Piece& rook = m_pieces[end - 2];
            rook.Position(end + 1);
            m_pieces[end + 1] = rook;
            m_pieces[end - 2] = Piece();
        }
    }
}



// ----- Fen -----

std::string Board::RecalculateFen(bool isCaptureOrPawn)
{
    char player = RecalculatePlayer();
    std::string castling = RecalculateCastling();
    std::string enPassant = RecalculateEnPassant();
    u32 halfMoves = RecalculateHalfMoves(isCaptureOrPawn);
    u32 fullMoves = RecalculateFullMoves();
    return Fen::GenerateFen(m_pieces, player, castling, enPassant, halfMoves, fullMoves);
}

char Board::RecalculatePlayer()
{
    return (m_playerColour == Enums::Colour::White ? 'w' : 'b');
}

std::string Board::RecalculateCastling()
{
    std::string castle = "";

    // Remove all castling if king moved
    if (m_pieces[4].Type() != Enums::Type::King) {
        m_castling &= ~((u8)Enums::Castling::White_King | (u8)Enums::Castling::White_Queen);
    }
    if (m_pieces[60].Type() != Enums::Type::King) {
        m_castling &= ~((u8)Enums::Castling::Black_King | (u8)Enums::Castling::Black_Queen);
    }

    // Individual castling checks
    if (m_pieces[7].Type() != Enums::Type::Rook) {
        m_castling &= ~((u8)Enums::Castling::White_King);
    }
    if (m_pieces[0].Type() != Enums::Type::Rook) {
        m_castling &= ~((u8)Enums::Castling::White_Queen);
    }
    if (m_pieces[63].Type() != Enums::Type::Rook) {
        m_castling &= ~((u8)Enums::Castling::Black_King);
    }
    if (m_pieces[56].Type() != Enums::Type::Rook) {
        m_castling &= ~((u8)Enums::Castling::Black_Queen);
    }

    if (m_castling & (u8)Enums::Castling::White_King) {
        castle += "K";
    }
    if (m_castling & (u8)Enums::Castling::White_Queen) {
        castle += "Q";
    }
    if (m_castling & (u8)Enums::Castling::Black_King) {
        castle += "k";
    }
    if (m_castling & (u8)Enums::Castling::Black_Queen) {
        castle += "q";
    }

    if (castle.length() == 0) {
        castle = "-";
    }

    return castle;
}

std::string Board::RecalculateEnPassant()
{
    std::string enPassant = "-";
    if (m_enPassant != INVALID_ENPASSANT) {
        enPassant = Convert::IndexToMove(m_enPassant);
    }

    return enPassant;
}

u32 Board::RecalculateHalfMoves(bool isCaptureOrPawn)
{
    if (isCaptureOrPawn) {
        return 0;
    }

    std::string_view strHalfMoves = m_fen.substr(m_fen.find(' ') + 1);
    strHalfMoves = strHalfMoves.substr(strHalfMoves.find(' ') + 1);
    strHalfMoves = strHalfMoves.substr(strHalfMoves.find(' ') + 1);
    strHalfMoves = strHalfMoves.substr(strHalfMoves.find(' ') + 1);

    u32 halfMoves = std::stoul(strHalfMoves.data());
    return (halfMoves + 1);

}

u32 Board::RecalculateFullMoves()
{
    std::string_view strHalfMoves = m_fen.substr(m_fen.find(' ') + 1);
    strHalfMoves = strHalfMoves.substr(strHalfMoves.find(' ') + 1);
    strHalfMoves = strHalfMoves.substr(strHalfMoves.find(' ') + 1);
    strHalfMoves = strHalfMoves.substr(strHalfMoves.find(' ') + 1);
    strHalfMoves = strHalfMoves.substr(strHalfMoves.find(' ') + 1);

    u32 fullMoves = std::stoul(strHalfMoves.data());
    if (m_playerColour == Enums::Colour::Black) {
        fullMoves++;
    }

    return fullMoves;
}

