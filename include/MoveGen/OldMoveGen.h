#pragma once

#include <span>

#include "Constants.h"
#include "Piece.h"

/**
 * @brief Generates moves for a given `Piece` and `Board` state.
 * @class OldMoveGen
 * @date 2026-06-29
 */
class OldMoveGen {
public:
    // Useful for determining if generated moves are invalid.
    static const BitBoard INVALID = 0x00'00'00'00'00'00'00'00;

    // ----- Creation / Destruction -----

    OldMoveGen();
    ~OldMoveGen() = default;

    // ----- Read -----

    // Returns the generated move bitboard.
    BitBoard GetMoves() const noexcept;

    // Determines if previous move generation results in checkmate.
    bool IsCheckmate() const noexcept;

    // Determines if previous move generation results in stalemate.
    bool IsStalemate() const noexcept;

    // ----- Update -----

    /**
     * @brief Generates moves for a given `Piece`.
     * @param pieces The current `Piece` list.
     * @param index The `Index` of the `Piece` to generate moves for.
     * @param castling The castling rights of the board.
     * @date 2026-06-29
     */
    void Generate(std::span<const Piece, 64> pieces, Index index, u8 castling);

private:
    // Data taken in
    const Piece* m_pieceList;
    Index        m_pieceIndex;
    u8           m_castling;

    // Calculation data
    bool     m_generatingAttacks, m_inCheck, m_inDoubleCheck, m_pinningPiece;
    Index    m_pinIndex;
    BitBoard m_attacks, m_pins, m_pinsHorz, m_pinsVert, m_pinsDiagUp, m_pinsDiagDown;
    BitBoard m_checkSquares, m_currentMoves;

    // Result data
    BitBoard m_validMoves;
    bool     m_isCheckmate, m_isStalemate;

    // General

    void Reset();
    int  PieceCompare(const Piece& lhs, const Piece& rhs);
    void CheckForCheckmate(Enums::Colour friendly);
    // void CheckForStalemate();

    // Attacks

    bool     IsSquareAttacked(Index index);
    BitBoard GenAttacks();
    void     ResetAttackPiece();

    // Checks

    void AddCheck();
    void AddCheckMoves(const Piece& piece);
    bool IsBlockCheck(Index index);

    // Pins

    void AddPiecePin(int pinDir);
    int  IsNewPin(const Piece& piece, const Piece& other, int pinDir);
    int  IsPiecePinned(const Piece& piece);

    // Verifying moves

    int AddMove(const Piece& piece, Index index);
    int AddPawnMove(const Piece& piece, Index index);

    // Move generation - standard

    BitBoard GenMoves(const Piece& piece);

    BitBoard GenBishop(const Piece& piece);
    BitBoard GenKing(const Piece& piece);
    BitBoard GenKnight(const Piece& piece);
    BitBoard GenPawn(const Piece& piece);
    BitBoard GenQueen(const Piece& piece);
    BitBoard GenRook(const Piece& piece);

    // Move generation - special

    BitBoard GenCastling(const Piece& piece);
    bool     IsValidForCastle(Index index);
    BitBoard GenSliding(const Piece& piece, i32 offset, Index mod);
};
