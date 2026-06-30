#include "Piece.h"

#include "Utils.h"

#include <print>

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

