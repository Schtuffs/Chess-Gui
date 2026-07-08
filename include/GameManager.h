#pragma once

#include <string>
#include <string_view>
#include <vector>

#include "Board.h"
#include "Button.h"

/**
 * @brief Manages game based workflows.
 * @class GameManager
 * @date 2026-07-01
 */
class GameManager {
public:

    // ----- Creation / Destruction -----
    
    /**
     * @brief Constructor.
     * @date 2026-07-01
     */
    GameManager();
    
    /**
     * @brief Deconstructor.
     * @date 2026-07-01
     */
    ~GameManager();

    // ----- Read -----

    /**
     * @brief Get the game fen state.
     * @return The fen state of the game.
     * @date 2026-07-01
     */
    std::string_view Fen() const noexcept;

    /**
     * @brief Get all moves that have been made throughout the game.
     * @return List of moves in long algebraic notation.
     * @date 2026-07-07
     */
    std::string Moves() const noexcept;
    
    // ----- Update -----

    /**
     * @brief Update the current game state.
     * @param isWhitePerspective If the game is currently shown from white's perspective.
     * @date 2026-07-05
     */
    void Update(bool isWhitePerspective);

private:
    std::vector<std::string> m_moves;
    std::vector<Button> m_buttons;
    Board m_board;
    BitBoard m_possibleMoves;
    bool m_isWhiteTurn, m_isWhiteAI, m_isBlackAI;

    void CheckMove(std::string& move);
    void OnButtonPress(Index index);
};

