#pragma once

#include <array>
#include <memory>
#include <string>
#include <string_view>

#include "Types/BitBoard.h"
#include "Types/Types.h"

typedef struct StateStore {
    // Copy
    u16 ply        = 0;
    u16 totalMoves = 0;

    // Calculate
    PieceType                   captured   = TYPE_NONE;
    BitBoard                    checkers   = 0;
    BitBoard                    attackRays = 0;
    Square                      enPassant  = SQ_BAD;
    std::string                 fen        = "";
    std::shared_ptr<StateStore> previous   = nullptr;
} StateStore;

class Position {
public:
    // ----- Creation / Destruction

    explicit Position(std::string_view fen) noexcept;
    explicit Position(const Position& pos) noexcept = default;
    ~Position()                                     = default;

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
    std::string Str() const noexcept;

    // ----- Update -----

    void MakeMove(Move move) noexcept;
    void UnmakeMove(Move move) noexcept;

private:
    std::array<BitBoard, TYPE_TOTAL>   m_bbType;
    std::array<BitBoard, COLOUR_TOTAL> m_bbColour;
    std::shared_ptr<StateStore>        m_state;

    Colour m_player;
    u8     m_castling;

    // ----- Read -----

    BitBoard  GetAttackRays() const noexcept;
    BitBoard  GetCheckers() const noexcept;
    PieceType GetType(Square sq) const noexcept;
    bool      IsAttacked(Square sq, BitBoard occupied, Colour us) const noexcept;

    // ----- Update -----

    void        ManageEnPassant(Move move) noexcept;
    PieceType   MovePiece(Move move) noexcept;
    std::string UpdateFen() const noexcept;
    void        UpdateMoves(Move move) noexcept;
};
