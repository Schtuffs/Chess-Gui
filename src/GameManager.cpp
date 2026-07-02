#include "GameManager.h"

#include <print>

#include "Fen.h"
#include "MoveGen.h"
#include "Settings.h"
#include "Utils.h"

// ----- Creation / Destruction -----

static void UpdateButtonsWithMove(std::vector<Button>& buttons, u64 moves)
{
    Vector3 grid = Utils::GridPositioning();
    for (int i = 0; i < 64; i++) {
        buttons[i].Dimension(Rectangle{grid.x + grid.z * (i % GRID_SIZE), grid.y + grid.z * (GRID_SIZE - (i / GRID_SIZE) - 1), grid.z, grid.z});
        
        u64 move = (moves >> i) & 1;
        if (move) {
            buttons[i].ColourInside({255, 0, 0, 255});
        }
        else {
            buttons[i].ColourInside((i + (i / GRID_SIZE)) % 2 == 1 ? BOARD_SQUARE_DARK_ALPHA : BOARD_SQUARE_LIGHT_ALPHA);
        }
    }
}

GameManager::GameManager()
    : m_board(Settings::s(Setting::GAME_STATE)), m_isWhiteTurn(true), m_isWhitePerspective(true)
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
            m_buttons.back().Thickness(Utils::Max(Utils::Min(GetScreenWidth(), GetScreenHeight()) / 300.f, 2.f));
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

void GameManager::Render()
{
    // Fix sizing on window change
    if (IsWindowResized()) {
        UpdateButtonsWithMove(m_buttons, m_possibleMoves);
    }

    if (IsKeyPressed(KEY_F)) {
        m_isWhitePerspective = !m_isWhitePerspective;
    }

    m_renderer.RenderBoard(BOARD_SQUARE_DARK, BOARD_SQUARE_LIGHT);

    static std::string move = "";
    for (size_t i = 0; i < m_buttons.size(); i++) {
        Button& button = m_buttons[i];
        
        if (button.IsHovered() || ((m_possibleMoves >> i) & 1)) {
            button.Render();
        }

        // Player is making moves
        if (button.IsClicked()) {
            move += Fen::IndexToMove(i);
            DebugPrintln("Move: {}", move);
            if (m_possibleMoves == 0) {
                Enums::Colour col = m_board.Pieces()[i].Colour();
                if (
                    (m_isWhiteTurn && col == Enums::Colour::Black) ||
                    (!m_isWhiteTurn && col == Enums::Colour::White)
                ) {
                    move.clear();
                    continue;
                }
                
                m_possibleMoves = MoveGen::Generate(m_board, m_board.Pieces()[i]);
                if (m_possibleMoves != 0) {
                    UpdateButtonsWithMove(m_buttons, m_possibleMoves);
                }
                else {
                    move.clear();
                }
            }
            else {
                if (m_board.MakeMove(move)) {
                    m_isWhiteTurn = !m_isWhiteTurn;
                }
                
                move.clear();
                m_possibleMoves = 0;
                UpdateButtonsWithMove(m_buttons, 0);
            }
            DebugPrintln("Piece: {}", m_board.Pieces()[24].ToString());
        }
    }

    m_renderer.RenderPieces(m_board.Fen(), m_isWhitePerspective);
}



// ----- Update -----

