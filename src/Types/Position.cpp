#include "Types/Position.h"

#include <print>

#include "MoveGen/MoveGen.h"
#include "Utils/Convert.h"
#include "Utils/Fen.h"

// ----- Creation ----- Destruction -----

Position::Position(std::string_view fen) noexcept : m_fen(fen)
{
    if (!Fen::IsValidFen(m_fen.data())) {
        m_fen = DEFAULT_FEN;
    }
    m_bbColour.fill(0);
    m_bbType.fill(0);

    // Get pieces
    u8 idx = 0;
    for (u64 rank = 7; rank < 8; rank--) {
        for (u64 file = 0; file < 8; file++) {
            Square sq = Square(rank * 8 + file);
            char   c  = m_fen[idx++];

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
    m_player = (m_fen[idx + 1] == 'w' ? WHITE : BLACK);

    // Get castling
    std::string_view castling = fen.substr(idx + 3);

    char c;
    idx = 0;
    while ((c = castling[idx++]) != ' ') {
        switch (c) {
        case 'K':
            m_castling |= static_cast<u8>(Enums::Castling::White_King);
            break;
        case 'Q':
            m_castling |= static_cast<u8>(Enums::Castling::White_Queen);
            break;
        case 'k':
            m_castling |= static_cast<u8>(Enums::Castling::Black_King);
            break;
        case 'q':
            m_castling |= static_cast<u8>(Enums::Castling::Black_Queen);
            break;
        }
    }

    // Get en passant
    std::string_view enPassant = castling.substr(idx);
    if (enPassant[0] != '-') {
        m_enPassant = Convert::StrToSquare(enPassant);
    }

    // // Get move counts
    // std::string_view halfMoves = enPassant.substr(enPassant.find(' ') + 1);
    // std::string_view fullMoves = halfMoves.substr(halfMoves.find(' ') + 1);
    // if (fullMoves.length() == 1 && fullMoves[0] == '1' &&
    //     m_playerColour == WHITE) {
    //     fen[fen.length() - 1] = '0';
    // }
}

// ----- Read -----

std::string Position::Fen() const noexcept { return m_fen; }

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
            if (!(Pieces(~m_player) & to) && !(to == m_enPassant)) {
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
Colour   Position::Player() const noexcept { return m_player; }

// ----- Read ----- Hidden -----

bool Position::IsAttacked(Square sq) const noexcept
{
    (void)sq;
    return false;
}

// ----- Update -----

void Position::MakeMove(Move move) noexcept
{
    ManageEnPassant(move);

    // Player making move BitBoard updates
    BitBoard rem = ~BitBoard(move.From());
    m_bbColour[Player()] &= rem;
    m_bbColour[Player()] |= move.To();

    // Type based BitBoard updates
    PieceType type;
    // clang-format off
    if (m_bbType[BISHOP] & move.From()) { m_bbType[BISHOP] &= rem; type = BISHOP; }
    if (m_bbType[KING]   & move.From()) { m_bbType[KING]   &= rem; type = KING; }
    if (m_bbType[KNIGHT] & move.From()) { m_bbType[KNIGHT] &= rem; type = KNIGHT; }
    if (m_bbType[PAWN]   & move.From()) { m_bbType[PAWN]   &= rem; type = PAWN; }
    if (m_bbType[QUEEN]  & move.From()) { m_bbType[QUEEN]  &= rem; type = QUEEN; }
    if (m_bbType[ROOK]   & move.From()) { m_bbType[ROOK]   &= rem; type = ROOK; }
    // clang-format on
    m_bbType[type] |= move.To();

    // Other player BitBoard update
    m_bbColour[~Player()] &= ~BitBoard(move.To());

    // Update gettable states
    m_player = ~m_player;

    UpdateFen(move);
}

void Position::UnmakeMove(Move move) noexcept
{
    // Nothing yet
    (void)move;
}

// ----- Update ----- Hidden -----

void Position::ManageEnPassant(Move move) noexcept
{
    // Check if this is a pawn
    if (!(Pieces(PAWN) & move.From())) {
        m_enPassant = SQ_BAD;
        return;
    }

    // If this was en passant
    if (move.To() == m_enPassant) {
        // Convert the offset to be always up/down by 8 but keep sign
        i8 off = (move.To() % 8) - (move.From() % 8);
        std::println("off: {}", off);
        BitBoard bb = Square(move.From() + off);
        std::println("off: {}{}", off, bb.Str());
        m_bbType[PAWN] &= ~bb;
        m_bbColour[~m_player] &= ~bb;
    }

    // En passant available
    m_enPassant = SQ_BAD;
    i8 travel   = (move.To() - move.From());
    if (travel == 16 || travel == -16) {
        m_enPassant = Square(move.From() + (travel / 2));
    }
}

void Position::UpdateFen(Move move) noexcept
{
    // Pieces
    std::string line, fen;
    for (u8 rank = 8; rank > 0; rank--) {
        u8 offset = 0;
        for (u8 file = 0; file < 8; file++) {
            Square sq = Square((rank - 1) * 8 + file);
            if (!(Pieces(WHITE) & sq) && !(Pieces(BLACK) & sq)) {
                offset++;
                continue;
            }

            if (offset) {
                line += offset + '0';
                offset = 0;
            }

            char c = '\0';
            if (Pieces(BISHOP) & sq) {
                c = 'b';
            } else if (Pieces(KING) & sq) {
                c = 'k';
            } else if (Pieces(KNIGHT) & sq) {
                c = 'n';
            } else if (Pieces(PAWN) & sq) {
                c = 'p';
            } else if (Pieces(QUEEN) & sq) {
                c = 'q';
            } else if (Pieces(ROOK) & sq) {
                c = 'r';
            }

            if (Pieces(WHITE) & sq) {
                c = toupper(c);
            }

            line += c;
        }

        fen += line;
        line.clear();

        if (offset) {
            fen += offset + '0';
        }

        if (rank != 1) {
            fen += "/";
        }
    }

    // Player to move
    fen += (m_player == WHITE ? " w " : " b ");

    // Castling
    if (m_castling == 0) {
        fen += '-';
    } else {
        if (m_castling & Enums::Castling::White_King) {
            fen += 'K';
        }
        if (m_castling & Enums::Castling::White_Queen) {
            fen += 'Q';
        }
        if (m_castling & Enums::Castling::Black_King) {
            fen += 'k';
        }
        if (m_castling & Enums::Castling::Black_Queen) {
            fen += 'q';
        }
    }
    fen += ' ';

    // En passant
    if (Pieces(PAWN) & move.To()) {
        if (std::abs(move.From() - move.To()) == 16) {
            fen += Convert::SquareToStr(Square(move.From() + (move.To() - move.From()) / 2));
        } else {
            fen += "- ";
        }
    } else {
        fen += "- ";
    }

    // Half moves

    // Whole moves

    m_fen = fen;
    std::println("Fen: {}", m_fen);
}
