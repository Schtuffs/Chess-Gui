#pragma once

#include "Constants.h"
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
     * @brief Generates moves for a given `Piece`.
     * @param pieces The current `Piece` list.
     * @param index The `Index` of the `Piece` to generate moves for.
     * @return The `BitBoard` representation of the moves.
     * @date 2026-06-29
     */
    BitBoard Generate(const Piece* pieces, Index index, u8 castling);

private:
    const Piece* m_pieceList;
    Index m_pieceIndex;
    u8 m_castling;

    bool m_generatingAttacks, m_inCheck, m_inDoubleCheck, m_pinningPiece;
    Index m_pinIndex;
    BitBoard m_attacks, m_pins, m_pinsHorz, m_pinsVert, m_pinsDiagUp, m_pinsDiagDown;
    BitBoard m_checkSquares, m_currentMoves;

    void Reset();

    void AddCheck();
    void AddCheckMoves(const Piece& piece);
    bool CheckForCheck(Index index);
    
    int GetPin(const Piece& piece);
    int CheckPin(const Piece& piece, const Piece& other, int pinDir);
    void UpdatePin(int pinDir);

    int AddMove(const Piece& piece, Index index);
    int AddPawnMove(const Piece& piece, Index index);
    BitBoard GenAttacks();

    BitBoard GenMoves(const Piece& piece);

    BitBoard GenSliding(const Piece& piece, i32 offset, Index mod);
    BitBoard GenBishop(const Piece& piece);
    BitBoard GenRook(const Piece& piece);
    BitBoard GenQueen(const Piece& piece);

    BitBoard GenKing(const Piece& piece);
    BitBoard GenKnight(const Piece& piece);
    BitBoard GenPawn(const Piece& piece);
    BitBoard GenPawnAttack(const Piece& piece);
};

