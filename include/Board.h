#pragma once

#include <string>
#include <string_view>

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
     * @brief Get the current fen gamestate.
     * @return The current game fen.
     * @date 2026-06-14
     */
    std::string_view Fen();

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
};

