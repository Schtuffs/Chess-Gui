#pragma once

#include <array>
#include <string_view>

#include "Types/BitBoard.h"
#include "Types/Types.h"

typedef struct StateStore {
    Piece       captured;
    StateStore* previous;
} StateStore;

class Position {
public:
    // ----- Creation / Destruction

    explicit Position(std::string_view fen) noexcept;
    ~Position() = default;

    // ----- Read -----

    u8          Castling() const noexcept;
    u8          Checkers() const noexcept;
    Square      EnPassant() const noexcept;
    std::string Fen() const noexcept;
    bool        IsCastleLegal(Square from, Square to) const noexcept;
    bool        IsLegal(Move move) const noexcept;
    BitBoard    Pieces() const noexcept;
    BitBoard    Pieces(Colour colour, PieceType type) const noexcept;
    BitBoard    Pieces(Colour colour) const noexcept;
    BitBoard    Pieces(PieceType type) const noexcept;
    Colour      Player() const noexcept;

    // ----- Update -----

    void MakeMove(Move move) noexcept;
    void UnmakeMove(Move move) noexcept;

private:
    std::array<BitBoard, TYPE_TOTAL>   m_bbType;
    std::array<BitBoard, COLOUR_TOTAL> m_bbColour;

    Colour      m_player;
    u8          m_castling;
    Square      m_enPassant;
    std::string m_fen;

    bool IsAttacked(Square sq, BitBoard occupied, Colour c) const noexcept;
    void ManageEnPassant(Move move) noexcept;
    void UpdateFen(Move move) noexcept;
};
