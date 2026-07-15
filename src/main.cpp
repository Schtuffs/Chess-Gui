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
#include <cstring>
#include <print>
#include <vector>

#include "raylib.h"

#include "Constants.h"
#include "Menus.h"
#include "Settings.h"
#include "Utils.h"

bool inDebugMode = false;

bool IsScreenSwapped(Enums::Screen screen)
{
    static Enums::Screen prev = Enums::Screen::Menu;
    bool value = prev != screen;
    prev = screen;
    return value;
}

int main(void)
{
    Utils::SetLogLevel(Utils::LogLevel::INFO);
    Settings::LoadSettings();

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

        if (IsScreenSwapped(currentScreen)) {
            if (currentScreen == Enums::Screen::Menu) {
                SetExitKey(KEY_ESCAPE);
            }
            else {
                SetExitKey(KEY_NULL);
            }
        }

        if (IsKeyPressed(KEY_ESCAPE)) {
            currentScreen = Enums::Screen::Menu;
        }
        
        switch(currentScreen) {
            case Enums::Screen::Quit: {
                shouldExitGame = true;
                break;
            }
            case Enums::Screen::Menu: {
                Menu::Main(currentScreen);
                break;
            }
            case Enums::Screen::NewGame: {
                Menu::NewGame(currentScreen);
                break;
            }
            case Enums::Screen::Game: {
                Menu::InGame(currentScreen);
                break;
            }
            case Enums::Screen::Settings: {
                Menu::Settings(currentScreen);
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
    
    Settings::SaveSettings();
    
    return 0;
}

