#pragma once

#include <string>
#include <string_view>
#include <vector>

#include "Board.h"
#include "Button.h"
#include "Renderer.h"

class GameManager {
public:

    // ----- Creation / Destruction -----
    
    GameManager();
    ~GameManager();

    // ----- Read -----

    std::string_view Fen() const noexcept;
    void Render();

    // ----- Update -----

private:
    std::vector<std::string> m_moves;
    std::vector<Button> m_buttons;
    Board m_board;
    Renderer m_renderer;
    u64 m_possibleMoves;
    bool m_isWhiteTurn, m_isWhitePerspective;
};

