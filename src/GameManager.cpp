#include "GameManager.h"

#include "Fen.h"
#include "MoveGen.h"
#include "Settings.h"
#include "Utils.h"

// ----- Creation / Destruction -----

static float DefaultButtonThickness()
{
    return (Utils::Max(Utils::Min(GetScreenWidth(), GetScreenHeight()) / 300.f, 2.f));
}

static void UpdateButtons(std::vector<Button>& buttons)
{
    Vector3 grid = Utils::GridPositioning();
    for (int i = 0; i < 64; i++) {
        buttons[i].Dimension(Rectangle{grid.x + grid.z * (i % GRID_SIZE), grid.y + grid.z * (GRID_SIZE - (i / GRID_SIZE) - 1), grid.z, grid.z});
        buttons[i].ColourInside((i + (i / GRID_SIZE)) % 2 == 1 ? BOARD_SQUARE_DARK_ALPHA : BOARD_SQUARE_LIGHT_ALPHA);
        buttons[i].Thickness(DefaultButtonThickness());
    }
}

static void UpdateButtonWithMove(Button& button, u64 moves, u64 index)
{
    if ((moves >> index) & 1) {
        button.ColourInside({255, 0, 0, 255});
        button.Thickness(DefaultButtonThickness());
    }
    else {
        button.ColourInside((index + (index / GRID_SIZE)) % 2 == 1 ? BOARD_SQUARE_DARK_ALPHA : BOARD_SQUARE_LIGHT_ALPHA);
        button.Thickness(0.f);
    }
}

GameManager::GameManager()
    : m_board(Settings::s(Setting::GAME_STATE)), m_isWhiteTurn(true)
{
    std::string_view fen = m_board.Fen();
    u64 index = fen.find(' ');
    fen = fen.substr(index + 1);

    if (fen[0] == 'b') {
        m_isWhiteTurn = false;
    }
    
    m_buttons.reserve(GRID_SIZE * GRID_SIZE);
    Vector3 grid = Utils::GridPositioning();
    for (u64 rank = 0; rank < GRID_SIZE; rank++) {
        for (u64 file = 0; file < GRID_SIZE; file++) {
            Color col = (((rank + file) % 2) == 0 ? BOARD_SQUARE_DARK_ALPHA : BOARD_SQUARE_LIGHT_ALPHA);
            m_buttons.emplace_back("", FontData{}, Rectangle{grid.x + grid.z * file, grid.y + grid.z * (GRID_SIZE - rank - 1), grid.z, grid.z}, col);
            m_buttons.back().Thickness(DefaultButtonThickness());
        }
    }
}

GameManager::~GameManager()
{}



// ----- Read -----

std::string_view GameManager::Fen() const noexcept
{
    return m_board.Fen();
}


void GameManager::Update(bool isWhitePerspective)
{
    // Fix sizing on window change
    if (IsWindowResized()) {
        UpdateButtons(m_buttons);
    }

    static std::string move = "";
    for (size_t i = 0; i < m_buttons.size(); i++) {
        u8 index = (u8)(isWhitePerspective ? i : 63 - i);

        Button& button = m_buttons[i];
        UpdateButtonWithMove(button, m_possibleMoves, index);
        
        if (button.IsHovered()) {
            button.Thickness(DefaultButtonThickness());
            button.Render();
        }

        if ((m_possibleMoves >> index) & 1){ 
            button.Render();
        }

        // Player is making moves
        if (button.IsClicked()) {
            move += Fen::IndexToMove(index);
            if (m_possibleMoves == 0) {
                Enums::Colour col = m_board.Pieces()[index].Colour();
                if (
                    (m_isWhiteTurn && col == Enums::Colour::Black) ||
                    (!m_isWhiteTurn && col == Enums::Colour::White)
                ) {
                    move.clear();
                    continue;
                }
                
                m_possibleMoves = MoveGen::Generate(m_board, m_board.Pieces()[index]);
                if (m_possibleMoves == 0) {
                    move.clear();
                }
            }
            else {
                if (m_board.MakeMove(move)) {
                    m_isWhiteTurn = !m_isWhiteTurn;
                }
                
                move.clear();
                m_possibleMoves = 0;
            }
        }
    }
}



// ----- Update -----

