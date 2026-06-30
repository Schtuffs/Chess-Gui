/*------------------------------

===== TODO =====

Renderer:
- Texture resizing for FixSize

Board:
- MakeMove needs to validate
- Needs piece movement
- Move calculation logic

------------------------------*/

#include <cstdio>
#include <print>
#include <vector>

#include "raylib.h"

#include "Board.h"
#include "Button.h"
#include "Constants.h"
#include "Menus.h"
#include "MoveGen.h"
#include "Renderer.h"
#include "Utils.h"

constexpr Color BOARD_SQUARE_LIGHT          = {175, 150, 120, 255};
constexpr Color BOARD_SQUARE_DARK           = {100, 75, 60, 255};
constexpr Color BOARD_SQUARE_LIGHT_ALPHA    = {175, 150, 120, 75};
constexpr Color BOARD_SQUARE_DARK_ALPHA     = {100, 75, 60, 75};
bool inDebugMode = false;

static void RenderMainMenu(Enums::Screen& screen)
{
    Menu::Main(screen);
}

static void RenderNewGameMenu(Enums::Screen& screen)
{
    Menu::NewGame(screen);
}

static void RenderSettings(Enums::Screen& screen)
{
    Menu::Settings(screen);
}

static std::vector<Button> SetupGameSquares()
{
    Vector3 grid = Utils::GridPositioning();
    std::vector<Button> buttons;
    buttons.reserve(GRID_SIZE * GRID_SIZE);

    for (u64 rank = 0; rank < GRID_SIZE; rank++) {
        for (u64 file = 0; file < GRID_SIZE; file++) {
            Color col = (((rank + file) % 2) == 0 ? BOARD_SQUARE_DARK_ALPHA : BOARD_SQUARE_LIGHT_ALPHA);
            buttons.emplace_back("", FontData{}, Rectangle{grid.x + grid.z * file, grid.y + grid.z * (GRID_SIZE - rank - 1), grid.z, grid.z}, col);
            buttons.back().Thickness(Utils::Max(Utils::Min(GetScreenWidth(), GetScreenHeight()) / 300.f, 2.f));
        }
    }

    return buttons;
}

static void UpdateButtonsWithMove(std::vector<Button>& buttons, u64 moves)
{
    for (int i = 0; i < 64; i++) {
        u64 move = (moves >> i) & 1;
        if (move) {
            buttons[i].ColourInside({255, 0, 0, 255});
        }
        else {
            buttons[i].ColourInside((i + (i / GRID_SIZE)) % 2 == 1 ? BOARD_SQUARE_DARK_ALPHA : BOARD_SQUARE_LIGHT_ALPHA);
        }
    }
}

static void RenderGame(Enums::Screen& screen)
{
    (void)screen;

    // Prepare states
    static Renderer renderer;
    static Board board(DEFAULT_FEN);
    static bool isWhitePerspective = true;
    static bool buttonsInitialized = false;
    static std::vector<Button> buttons;

    // Initialize the buttons only once
    if (!buttonsInitialized) {
        buttonsInitialized = true;
        buttons = SetupGameSquares();
    }

    // Main render logic
    
    if (IsKeyPressed(KEY_F)) {
        isWhitePerspective = !isWhitePerspective;
    }
    
    renderer.RenderBoard(BOARD_SQUARE_DARK, BOARD_SQUARE_LIGHT);
    
    
    // Check buttons
    static std::string move = "";
    // static bool pieceSelected = false;
    u64 moves = 0;
    for (size_t i = 0; i < buttons.size(); i++) {
        Button& button = buttons[i];

        // Fix sizing on window change
        if (IsWindowResized()) {
            buttons = SetupGameSquares();
            UpdateButtonsWithMove(buttons, moves);
        }

        if (button.IsHovered() || ((moves >> i) & 1)) {
            button.Render();
        }

        // Player is making moves
        if (button.IsClicked()) {
            move += (char)((i % GRID_SIZE) + 'a') + std::to_string((i / GRID_SIZE) + 1);
            // if (!pieceSelected) {
                moves = MoveGen::Generate(board, board.Pieces()[i]);
                if (moves != 0) {
                    UpdateButtonsWithMove(buttons, moves);
                }
                else {
                    move.clear();
                }
            // }
            // else {
            //     buttons = SetupGameSquares();
            // }

            // if (move.length() == 4) {
            //     board.MakeMove(move);
            //     move = "";
            //     pieceSelected = false;
            // }
        }
    }
    renderer.RenderPieces(board.Fen(), isWhitePerspective);
}
#define heh(...) /* __VA_ARGS__ */ do {} while (false)
int main(void)
{
    Utils::SetLogLevel(Utils::LogLevel::DEBUG);

    // Prepare window
    SetTraceLogLevel(LOG_WARNING);
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(120, 120, "Chess Engine");
    SetWindowMinSize(200, 200);
    SetTargetFPS(60);

    // Main loop
    Enums::Screen currentScreen = Enums::Screen::Menu;
    bool shouldExitGame = false;
    while (!WindowShouldClose() && !shouldExitGame) {
        // Drawing
        BeginDrawing();
        ClearBackground(BLACK);

        if (IsKeyPressed(KEY_D)) {
            inDebugMode = !inDebugMode;
        }
        
        switch(currentScreen) {
            case Enums::Screen::Quit: {
                shouldExitGame = true;
                break;
            }
            case Enums::Screen::Menu: {
                RenderMainMenu(currentScreen);
                break;
            }
            case Enums::Screen::NewGame: {
                RenderNewGameMenu(currentScreen);
                break;
            }
            case Enums::Screen::Game: {
                RenderGame(currentScreen);
                break;
            }
            case Enums::Screen::Settings: {
                RenderSettings(currentScreen);
                break;
            }
            default: {
                char text[30]{};
                snprintf(text, sizeof(text), "Invalid screen selected: %d", (int)currentScreen);
                int fontSize = Utils::Max(GetScreenWidth() / 100, 20);

                Font font = GetFontDefault();
                Vector2 pos = Utils::CenterText(text, font, fontSize, {GetScreenWidth() / 2.f, GetScreenHeight() / 2.f});

                DrawText(text, pos.x, pos.y, fontSize, WHITE);
                break;
            }
        }

        if (inDebugMode) {
            DrawFPS(0, 0);
        }

        EndDrawing();
    }
    
    // Cleanup
    CloseWindow();
    
    return 0;
}

