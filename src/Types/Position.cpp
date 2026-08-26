#include "Types/Position.h"

#include <bit>
#include <print>

#include "MoveGen/Magic.h"
#include "MoveGen/MoveGen.h"
#include "Utils/Convert.h"
#include "Utils/Fen.h"
#include "Utils/Utils.h"

// ----- Creation ----- Destruction -----

Position::Position(std::string_view fen) noexcept : m_fen(fen)
{
    if (!Fen::IsValidFen(m_fen.data())) {
        m_fen = Fen::DEFAULT;
    }
    m_bbColour.fill(0ull);
    m_bbType.fill(0ull);

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
    std::string_view castling = m_fen.substr(idx + 3);

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
    m_enPassant                = SQ_BAD;
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

u8 Position::Castling() const noexcept { return m_castling; }

u8 Position::Checkers() const noexcept { return m_checkers.Count(); }

Square Position::EnPassant() const noexcept { return m_enPassant; }

std::string Position::Fen() const noexcept { return m_fen; }

bool Position::IsCastleLegal(Square from, Square to) const noexcept
{
    Colour    us  = Player();
    Direction dir = (to > from ? EAST : WEST);
    BitBoard  bb  = from;

    if (!Utils::IsValidSquare(from)) {
        return false;
    }

    if (!Utils::IsValidSquare(to)) {
        return false;
    }

    // Kingside
    if (to > from) {
        bb = bb << 1;
        bb |= bb << 1;
    } else {
        bb = bb >> 1;
        bb |= bb >> 1;
    }

    // Can't have other pieces there
    if (Pieces() & bb) {
        return false;
    }

    // No attacks allowed
    if (IsAttacked(from, Pieces(), ~us)) {
        return false;
    }
    if (IsAttacked(from + dir, Pieces(), ~us)) {
        return false;
    }
    if (IsAttacked(to, Pieces(), ~us)) {
        return false;
    }

    return true;
}

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
            // Attacks on castle path
            if (IsAttacked(sq, Pieces(), ~us)) {
                return false;
            }

            // Piece on castle path
            if (sq != from && Pieces() & sq) {
                return false;
            }
        }
    }

    // King
    if (m_bbType[KING] & from) {
        return !IsAttacked(to, Pieces() ^ from, ~us);
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

PieceType Position::GetType(Square sq) const noexcept
{
    // clang-format off
    if      (Pieces(BISHOP) & sq) { return    BISHOP; }
    else if (Pieces(KING)   & sq) { return      KING; }
    else if (Pieces(KNIGHT) & sq) { return    KNIGHT; }
    else if (Pieces(PAWN)   & sq) { return      PAWN; }
    else if (Pieces(QUEEN)  & sq) { return     QUEEN; }
    else if (Pieces(ROOK)   & sq) { return      ROOK; }
    else                          { return TYPE_NONE; }
    // clang-format on
}

bool Position::IsAttacked(Square sq, BitBoard occupied, Colour attacker) const noexcept
{
    return (Magic::GetAttacks<ROOK>(sq, occupied) &
            (Pieces(attacker, ROOK) | Pieces(attacker, QUEEN))) ||
           (Magic::GetAttacks<BISHOP>(sq, occupied) &
            (Pieces(attacker, BISHOP) | Pieces(attacker, QUEEN))) ||
           (Magic::GetAttacks<PAWN>(sq, occupied, ~attacker) & Pieces(attacker, PAWN)) ||
           (Magic::GetAttacks<KNIGHT>(sq, occupied) & Pieces(attacker, KNIGHT)) ||
           (Magic::GetAttacks<KING>(sq, occupied) & Pieces(attacker, KING));
}

// ----- Update -----

void Position::MakeMove(Move move) noexcept
{
    auto store      = std::make_shared<StateStore>();
    store->previous = m_state;
    m_state         = store;

    // Colour us   = Player();
    Square from = move.From();
    Square to   = move.To();

    // Moving
    m_state->captured = MovePiece(move);
    if (move.IsCastle()) {
        // Move the rook
        if (to > from) {
            m_bbType[ROOK] &= ~Square(to + 1);
            m_bbType[ROOK] |= Square(to - 1);
        } else {
            m_bbType[ROOK] &= ~Square(to - 2);
            m_bbType[ROOK] |= Square(to + 1);
        }
    }

    // Update gettable states
    m_player = ~m_player;
    UpdateFen(move);
}

void Position::UnmakeMove(Move move) noexcept
{
    m_player    = ~m_player;
    Colour us   = Player();
    Square from = move.To();
    Square to   = move.From();

    MovePiece(Move::Make(from, to));

    PieceType capturedType = m_state->captured;
    if (capturedType != TYPE_NONE) {
        m_bbColour[~us] |= to;
        m_bbType[capturedType] |= from;
    }

    m_state = m_state->previous;
    UpdateFen(move);
}

// ----- Update ----- Hidden -----

void Position::CalculateCheckers() noexcept
{
    Colour us  = Player();
    Square ksq = Pieces(us, KING).PopLSB();

    m_checkers |=
        (Magic::GetAttacks<BISHOP>(ksq, Pieces(), us) & (Pieces(~us, BISHOP) | Pieces(~us, QUEEN)));
    m_checkers |= (Magic::GetAttacks<KNIGHT>(ksq, Pieces(), us) & Pieces(~us, KNIGHT));
    m_checkers |= (Magic::GetAttacks<PAWN>(ksq, Pieces(), us) & Pieces(~us, PAWN));
    m_checkers |=
        (Magic::GetAttacks<ROOK>(ksq, Pieces(), us) & (Pieces(~us, ROOK) | Pieces(~us, QUEEN)));
}

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
        i32      off = (move.To() % 8) - (move.From() % 8);
        BitBoard bb  = Square(move.From() + off);
        m_bbType[PAWN] &= ~bb;
        m_bbColour[~m_player] &= ~bb;
    }

    // En passant available
    m_enPassant = SQ_BAD;
    i8 travel   = (move.To() - move.From());
    if (std::abs(travel) == 16) {
        m_enPassant = Square(move.From() + (travel / 2));
    }
}

PieceType Position::MovePiece(Move move) noexcept
{
    Colour us   = Player();
    Square from = move.From();
    Square to   = move.To();

    // Type based BitBoard updates
    BitBoard fRem = ~BitBoard(from);
    BitBoard tRem = ~BitBoard(to);

    // Get Piece types
    PieceType movedType    = GetType(from);
    PieceType capturedType = GetType(to);

    // Remove from us
    m_bbType[movedType] &= fRem;
    m_bbColour[us] &= fRem;

    // Remove from them
    m_bbType[capturedType] &= tRem;
    m_bbColour[~us] &= tRem;

    // Add to us
    m_bbType[movedType] |= to;
    m_bbColour[us] |= to;

    return capturedType;
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
    if (move.IsEnPassant()) {
        fen += Convert::SquareToStr(Square(move.From() + (move.To() - move.From()) / 2));
    } else {
        fen += "- ";
    }

    // Half moves

    // Whole moves

    m_fen = fen;
}
