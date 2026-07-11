#pragma once

#include "Constants.h"
#include "Board.h"
#include "Piece.h"

/**
 * @brief Generates moves for a given `Piece` and `Board` state.
 * @class MoveGen
 * @date 2026-06-29
 */
class MoveGen {
public:
    static const BitBoard INVALID      = 0x00'00'00'00'00'00'00'00;
    static const BitBoard CHECKMATE    = 0xff'ff'ff'ff'ff'ff'ff'ff;
    
    // ----- Creation / Destruction -----

    MoveGen();
    ~MoveGen() = default;
    
    // ----- Read -----

    // ----- Update -----
    
    /**
     * @brief Generates moves for a given `Piece` and `Board` state.
     * @param board The current `Board` state.
     * @param piece The `Piece` to generate moves for.
     * @return The `BitBoard` representation of the moves.
     * @date 2026-06-29
     */
    BitBoard Generate(const Board& board, const Piece& piece);

private:
    bool m_inCheck, m_inDoubleCheck, m_generatingAttacks;
    const Board* m_board;

    int AddMove(const Piece& piece, Index index, BitBoard& bb);
    BitBoard GenAttacks();

    BitBoard GenTypeSelection(const Piece& piece);

    BitBoard GenBishop(const Piece& piece);
    BitBoard GenRook(const Piece& piece);
    BitBoard GenQueen(const Piece& piece);

    BitBoard GenKing(const Piece& piece);
    BitBoard GenKnight(const Piece& piece);
    BitBoard GenPawn(const Piece& piece);
    BitBoard GenPawnAttack(const Piece& piece);
};

