#pragma once

#include <array>
#include <span>
#include <string>
#include <string_view>

#include "MoveGen.h"
#include "Piece.h"

/**
 * @brief Holds the state of the game board.
 * @class Board
 * @date 2026-06-14
 */
class Board {
public:
    // ----- Creation / Destruction -----

    // Create board with given fen.
    Board(std::string_view fen);

    // Free memory.
    ~Board();

    // ----- Read -----
    
    // Gets castling rights for given king colour.
    u8 Castling(Enums::Colour colour) const noexcept;
    
    // Get current gamestate fen.
    std::string_view Fen() const noexcept;
    
    // Get the board's piece list.
    std::span<const Piece, 64> Pieces() const noexcept;
    
    // Get current board player colour.
    Enums::Colour Player() const noexcept;

    // ----- Update -----
    
    // Try to play given move (long algebraic notation).
    bool MakeMove(std::string_view move);
    
    // Promotes a pawn on the board to given type.
    bool PromotePawn(Index index, Enums::Type type);
    
private:
    std::string m_fen;
    std::array<Piece, 64> m_pieces;
    u8 m_castling;
    Index m_enPassant;
    Enums::Colour m_playerColour;
    MoveGen m_moveGen;

    bool ValidateMove(Index start, Index end);
    Piece MovePiece(Index start, Index end);
    void MoveEnPassant(const Piece& piece, const Piece& other);
    void MoveCastling();

    char RecalculatePlayer();
    std::string RecalculateCastling(Index start, Index end);
    std::string RecalculateEnPassant(Index start, Index end);
    u32 RecalculateHalfMoves(bool isCaptureOrPawn);
    u32 RecalculateFullMoves();
};

