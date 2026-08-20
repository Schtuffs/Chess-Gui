#pragma once

#include <array>
#include <string_view>

#include "Types/BitBoard.h"
#include "Types/Types.h"

class Position {
public:
    explicit Position(std::string_view fen) noexcept;
    ~Position() = default;

    std::string Fen() const noexcept;
    bool        IsLegal(Move move) const noexcept;
    BitBoard    Pieces() const noexcept;
    BitBoard    Pieces(Colour colour, PieceType type) const noexcept;
    BitBoard    Pieces(Colour colour) const noexcept;
    BitBoard    Pieces(PieceType type) const noexcept;
    Colour      Player() const noexcept;

private:
    std::array<BitBoard, TYPE_TOTAL>   m_bbType;
    std::array<BitBoard, COLOUR_TOTAL> m_bbColour;
    bool                               m_isWhiteTurn, m_isValid;
    std::string                        m_fen;

    bool IsAttacked(Square sq) const noexcept;
};
