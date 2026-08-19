#include "Types/Position.h"

#include <print>

#include "MoveGen/MoveGen.h"
#include "Utils/Fen.h"

// ----- Creation ----- Destruction -----

Position::Position(std::string_view fen) noexcept
{
    if (!Fen::IsValidFen(fen.data())) {
        fen = DEFAULT_FEN;
    }
    m_bbColour.fill(0);
    m_bbType.fill(0);

    // Get pieces
    u8 idx = 0;
    for (u64 rank = 8 - 1; rank < 8; rank--) {
        for (u64 file = 0; file < 8; file++) {
            Square sq = Square(rank * 8 + file);
            char   c  = fen[idx++];

            if (isdigit(c)) {
                file += c - '0' - 1;
                continue;
            }

            if (c == '/') {
                rank++;
                break;
            }

            switch (c) {
            case 'b':
            case 'B':
                m_bbType[BISHOP] |= sq;
                (isupper(c) ? m_bbColour[WHITE] : m_bbColour[BLACK]) |= sq;
                break;
            case 'k':
            case 'K':
                m_bbType[KING] |= sq;
                (isupper(c) ? m_bbColour[WHITE] : m_bbColour[BLACK]) |= sq;
                break;
            case 'n':
            case 'N':
                m_bbType[KNIGHT] |= sq;
                (isupper(c) ? m_bbColour[WHITE] : m_bbColour[BLACK]) |= sq;
                break;
            case 'p':
            case 'P':
                m_bbType[PAWN] |= sq;
                (isupper(c) ? m_bbColour[WHITE] : m_bbColour[BLACK]) |= sq;
                break;
            case 'q':
            case 'Q':
                m_bbType[QUEEN] |= sq;
                (isupper(c) ? m_bbColour[WHITE] : m_bbColour[BLACK]) |= sq;
                break;
            case 'r':
            case 'R':
                m_bbType[ROOK] |= sq;
                (isupper(c) ? m_bbColour[WHITE] : m_bbColour[BLACK]) |= sq;
                break;
            }
        }
    }

    // Get player
    m_isWhiteTurn = (fen[idx + 1] == 'w');

    // // Get castling
    // std::string_view castling = fen.substr(sq + 3);

    // char c;
    // sq = 0;
    // while ((c = castling[sq++]) != ' ') {
    //     switch (c) {
    //     case 'K':
    //         m_castling |= static_cast<u8>(Enums::Castling::White_King);
    //         break;
    //     case 'Q':
    //         m_castling |= static_cast<u8>(Enums::Castling::White_Queen);
    //         break;
    //     case 'k':
    //         m_castling |= static_cast<u8>(Enums::Castling::Black_King);
    //         break;
    //     case 'q':
    //         m_castling |= static_cast<u8>(Enums::Castling::Black_Queen);
    //         break;
    //     }
    // }

    // // Get en passant
    // std::string_view enPassant = castling.substr(sq);
    // if (enPassant[0] != '-') {
    //     m_enPassant           = Convert::MoveToIndex(enPassant);
    //     m_pieces[m_enPassant] = Piece(m_enPassant);
    // }

    // // Get move counts
    // std::string_view halfMoves = enPassant.substr(enPassant.find(' ') + 1);
    // std::string_view fullMoves = halfMoves.substr(halfMoves.find(' ') + 1);
    // if (fullMoves.length() == 1 && fullMoves[0] == '1' &&
    //     m_playerColour == WHITE) {
    //     fen[fen.length() - 1] = '0';
    // }
}

// ----- Read -----

bool Position::IsLegal(Move move) const noexcept
{
    Colour us   = Player();
    Square from = move.From();
    Square to   = move.To();

    // Needs to be from us
    if (!(m_bbColour[us] & from)) {
        return false;
    }
    
    // Cannot be to us
    if ((m_bbColour[us] & to)) {
        return false;
    }
    
    // Check pawn
    if (m_bbType[PAWN] & from) {
        Square fromFile = from % 8;
        Square toFile   = to % 8;
        
        // Check attack gets enemy
        if (fromFile != toFile) {
            if (!(m_bbColour[~us] & to)) {
                return false;
            }
        }
        return true;
    }
    
    // Special castling
    if (move.IsCastle()) {
        Direction dir = (to > from ? EAST : WEST);
        for (Square sq = from; sq != to; sq += dir) {
            if (IsAttacked(sq)) {
                return false;
            }
        }
    }

    return true;
}

BitBoard Position::Pieces() const noexcept { return (m_bbColour[WHITE] | m_bbColour[BLACK]); }
BitBoard Position::Pieces(Colour colour, PieceType type) const noexcept
{
    return (m_bbColour[colour] & m_bbType[type]);
}
BitBoard Position::Pieces(Colour colour) const noexcept { return m_bbColour[colour]; }
BitBoard Position::Pieces(PieceType type) const noexcept { return m_bbType[type]; }
Colour   Position::Player() const noexcept { return (m_isWhiteTurn ? WHITE : BLACK); }

// ----- Read ----- Hidden -----

bool Position::IsAttacked(Square sq) const noexcept
{
    (void)sq;
    return false;
}

// ----- Update -----
