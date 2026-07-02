#include "Piece.h"

#include <print>

#include "Fen.h"
#include "Utils.h"

// ----- Creation / Destruction -----

Piece::Piece()
    : m_colour(Enums::Colour::Invalid), m_type(Enums::Type::Invalid), m_pos(UINT8_MAX), m_isValid(false)
{}

Piece::Piece(Enums::Colour colour, Enums::Type type, u8 pos)
    : m_colour(colour), m_type(type), m_pos(pos), m_isValid(true)
{}

Piece::~Piece()
{}



// ----- Read -----

Enums::Colour Piece::Colour() const noexcept
{
    return m_colour;
}

Enums::Type Piece::Type() const noexcept
{
    return m_type;
}

u64 Piece::Position() const noexcept
{
    return ((u64)1 << m_pos);
}

bool Piece::IsValid() const noexcept
{
    return m_isValid;
}

std::string Piece::ToString() const noexcept
{
    std::string str = "{ ";
    str += Enums::ToString::Colour[(u64)m_colour];
    str += ", ";
    str += Enums::ToString::Type[(u64)m_type];
    str += ", ";
    str += Fen::IndexToMove(m_pos);
    str += " }";
    return str;
}

char Piece::AsChar() const noexcept
{
    bool isUpper = false;
    if (m_colour == Enums::Colour::White) {
        isUpper = true;
    }

    char c = 0;
    switch (m_type) {
    case Enums::Type::Bishop:
        c = 'b';
        break;
    case Enums::Type::King:
        c = 'k';
        break;
    case Enums::Type::Knight:
        c = 'n';
        break;
    case Enums::Type::Queen:
        c = 'q';
        break;
    case Enums::Type::Pawn:
        c = 'p';
        break;
    case Enums::Type::Rook:
        c = 'r';
        break;
    default:
        return c;
    }

    if (isUpper) {
        c = toupper(c);
    }

    return c;
}



// ----- Update -----

void Piece::Position(u8 pos)
{
    m_pos = pos;
}

