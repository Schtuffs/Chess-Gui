#pragma once

#include <array>
#include <span>

#include "Board.h"
#include "Constants.h"
#include "Piece.h"

class MoveGen {
public:
    // Useful for determining if generated moves are invalid.
    static constexpr BitBoard INVALID = 0x00'00'00'00'00'00'00'00;

    // ----- Creation / Destruction -----

    MoveGen();
    ~MoveGen() = default;

    // ----- Read -----

    BitBoard GetMoves(Index index) const noexcept;
    bool     IsCheckmate() const noexcept;
    bool     IsStalemate() const noexcept;

    // ----- Update -----

    void Generate(const Board& board, Enums::Colour colour);

private:
    // Passed parameters
    Board         m_board;
    Enums::Colour m_genColour;

    // Calculated items
    BitBoard                 m_friendly;
    BitBoard                 m_enemies;
    std::array<BitBoard, 64> m_pseudoLegal;

    // Output items
    bool                     m_hasGenerated;
    std::array<BitBoard, 64> m_legal;
    bool                     m_isCheckmate, m_isStalemate;

    void Reset();
    void SetupPieceBoards();

    BitBoard GenMoves(const Piece& piece) const noexcept;
    BitBoard GenBishop(const Piece& piece) const noexcept;
    BitBoard GenKing(const Piece& piece) const noexcept;
    BitBoard GenKnight(const Piece& piece) const noexcept;
    BitBoard GenPawn(const Piece& piece) const noexcept;
    BitBoard GenQueen(const Piece& piece) const noexcept;
    BitBoard GenRook(const Piece& piece) const noexcept;

    void     GenPseudoLegal();
    BitBoard IterateMoves(const Piece& piece, BitBoard moves);

    void GenLegal();
};
