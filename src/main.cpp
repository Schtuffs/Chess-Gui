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
#include <vector>

#include "raylib.h"

#include "Board.h"
#include "Button.h"
#include "Constants.h"
#include "Renderer.h"
#include "Utils.h"
#include "Menus.h"

constexpr Color BOARD_SQUARE_LIGHT      = {175, 150, 120, 255};
constexpr Color BOARD_SQUARE_DARK       = {100, 75, 60, 255};
bool inDebugMode = false;

void RenderMainMenu(Enums::Screen& screen)
{
    Menu::Main(screen);
}

void RenderNewGameMenu(Enums::Screen& screen)
{
    Menu::NewGame(screen);
}

void RenderSettings(Enums::Screen& screen)
{
    Menu::Settings(screen);
}

void RenderGame(Enums::Screen& screen)
{
    (void)screen;

    // Prepare states
    static Renderer renderer;
    static Board board(DEFAULT_FEN);
    static bool isWhitePerspective = true, buttonsInitialized = false;
    static std::vector<Button> buttons;

    // Initialize the buttons only once
    if (!buttonsInitialized) {
        buttonsInitialized = true;
        buttons.reserve(GRID_SIZE * GRID_SIZE);

        Vector3 grid = Utils::GridPositioning();
        
        // Create the squares
        for (int rank = 0; rank < GRID_SIZE; rank++) {
            for (int file = 0; file < GRID_SIZE; file++) {
                Color col = (((rank + file) % 2) == 0 ? BOARD_SQUARE_DARK : BOARD_SQUARE_LIGHT);
                col.a = 50;
                buttons.emplace_back("", FontData{}, Rectangle{grid.x + grid.z * file, grid.y + grid.z * rank, grid.z, grid.z}, col);
                buttons[rank * GRID_SIZE + file].Thickness(Utils::Max(Utils::Min(GetScreenWidth(), GetScreenHeight()) / 100.f, 1.f));
            }
        }
    }

    // Main render logic
    
    if (IsKeyPressed(KEY_F)) {
        isWhitePerspective = !isWhitePerspective;
    }
    
    renderer.RenderBoard(BOARD_SQUARE_DARK, BOARD_SQUARE_LIGHT);
    renderer.RenderPieces(DEFAULT_FEN, isWhitePerspective);
    
    // Check buttons
    constexpr const char* NUM_TO_LETTER = "abcdefgh";
    static std::string move = "";
    for (size_t i = 0; i < buttons.size(); i++) {
        Button& button = buttons[i];

        // Fix sizing on window change
        if (IsWindowResized()) {
            Vector3 grid = Utils::GridPositioning();
            button.Dimension(Rectangle{grid.x + grid.z * (i % GRID_SIZE), grid.y + grid.z * (i / GRID_SIZE), grid.z, grid.z});
            button.Thickness(Utils::Max(Utils::Min(GetScreenWidth(), GetScreenHeight()) / 300.f, 2.f));
        }

        if (button.IsHovered()) {
            button.Render();
        }

        // Player is making moves
        if (button.IsClicked()) {
            move += NUM_TO_LETTER[i % GRID_SIZE] + std::to_string(GRID_SIZE - (i / GRID_SIZE));
            std::printf("MOVE: %s\n", move.c_str());

            if (move.length() == 4) {
                board.MakeMove(move);
                move = "";
            }
        }
    }
}

int main(void)
{
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
                snprintf(text, sizeof(text), "Invalid screen selected: %d", currentScreen);
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

