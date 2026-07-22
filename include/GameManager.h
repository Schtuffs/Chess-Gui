#pragma once

#include <string>
#include <string_view>
#include <vector>

#include "Board.h"
#include "MoveGen.h"

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
    GameManager(std::string_view fen);
    
    /**
     * @brief Deconstructor.
     * @date 2026-07-01
     */
    ~GameManager();

    // ----- Read -----

    /**
     * @brief Get all moves that have been made throughout the game.
     * @return String of moves in long algebraic notation.
     * @date 2026-07-07
     */
    std::string AllMoves() const noexcept;

    /**
     * @brief Get the game fen state.
     * @return The fen state of the game.
     * @date 2026-07-01
     */
    std::string_view Fen() const noexcept;

    /**
     * @brief Get the valid moves from selected `Piece`.
     * @return The `BitBoard` moves.
     * @date 2026-07-08
     */
    BitBoard Moves() const noexcept;
    
    // ----- Update -----

    /**
     * @brief Update the current game state.
     * @param move A potential move that a player is trying to make.
     * @date 2026-07-05
     */
    void Update(std::string_view move);

private:
    std::vector<std::string> m_moves;
    Board m_board;
    MoveGen m_moveGen;
    BitBoard m_possibleMoves;
    Index m_promotionSquare;
    bool m_isWhiteTurn, m_isWhiteAI, m_isBlackAI;

    void Update(std::string_view move, bool tryReselect);

    bool CheckMove(std::string& move);
    bool CheckPieceSelectable(Index index);
    void OnValidMove(std::string_view move);

    void CheckForPromotion(std::string_view move);
    void ManagePromotion(std::string_view move);
    
    void CheckForCheckmate();
};

