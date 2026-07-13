#pragma once

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

    /**
     * @brief Create board with given fen.
     * @param fen The fen position to start the game with.
     * @date 2026-06-14
     */
    Board(std::string_view fen);

    /**
     * @brief Frees memory.
     * @date 2026-06-14
     */
    ~Board();

    // ----- Read -----
    
    /**
     * @brief Get the castling rights for specified `Enums::Colour`.
     * @param colour The `Enums::Colour` castling rights to get.
     * @return The current castling rights for the king masked with `Enums::Castling`.
     * @date 2026-07-03
     */
    u8 Castling(Enums::Colour colour) const noexcept;
    
    /**
     * @brief Get the current fen gamestate.
     * @return The current game fen.
     * @date 2026-06-14
     */
    std::string_view Fen() const noexcept;
    
    /**
     * @brief Get the boards `Piece` list, including invalid.
     * @return The `Piece` list.
     * @date 2026-06-14
     */
    const Piece* Pieces() const noexcept;
    
    /**
     * @brief Get the current player to move.
     * @return The current players `Enums::Colour`.
     * @date 2026-07-03
     */
    Enums::Colour Player() const noexcept;

    // ----- Update -----
    
    /**
     * @brief Attempt to make a move on the board.
     * @param move A move in long algebraic notation.
     * @return `true` on move successfully made.
     * @date 2026-06-14
     */
    bool MakeMove(std::string_view move);
    
private:
    std::string m_fen;
    Piece m_pieces[64];
    u8 m_castling, m_enPassant;
    Enums::Colour m_playerColour;
    MoveGen m_moveGen;

    Piece MovePiece(Index start, Index end);
    bool ValidateMove(Index start, Index end);

    char RecalculatePlayer();
    std::string RecalculateCastling(Index start, Index end);
    std::string RecalculateEnPassant(Index start, Index end);
    u32 RecalculateHalfMoves(bool isCaptureOrPawn);
    u32 RecalculateFullMoves();
};

