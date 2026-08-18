#include <cstdio>
#include <cstring>
#include <vector>

#include "raylib.h"

#include "Pipes/Pipes.h"
#include "State/Menus.h"
#include "Utils/Constants.h"
#include "Utils/Settings.h"
#include "Utils/Utils.h"

bool inDebugMode = false;

bool IsScreenSwapped(Enums::Screen screen)
{
    static Enums::Screen prev  = Enums::Screen::Menu;
    bool                 value = prev != screen;
    prev                       = screen;
    return value;
}

int main(void)
{
#ifdef NDEBUG
    Utils::SetLogLevel(Utils::LogLevel::PRINT);
#else
    Utils::SetLogLevel(Utils::LogLevel::INFO);
#endif
    Settings::LoadSettings();

    // Prepare window
    SetTraceLogLevel(LOG_WARNING);
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(120, 120, "Chess Engine");
    SetWindowMinSize(200, 200);
    SetTargetFPS(60);

    // Icon
    Image icon = LoadImage((std::string(PATH_RESOURCES) + "/ChessGui.png").c_str());
    SetWindowIcon(icon);

    // Main loop
    Enums::Screen currentScreen  = Enums::Screen::Menu;
    bool          shouldExitGame = false;
    while (!WindowShouldClose() && !shouldExitGame) {
        if (IsKeyPressed(KEY_D)) {
            inDebugMode = !inDebugMode;
        }

        if (IsScreenSwapped(currentScreen)) {
            if (currentScreen == Enums::Screen::Menu) {
                SetExitKey(KEY_ESCAPE);
            } else {
                SetExitKey(KEY_NULL);
            }
        }

        // Drawing
        BeginDrawing();
        ClearBackground(BLACK);

        switch (currentScreen) {
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
        case Enums::Screen::GameCreation: {
            Menu::CreateGame(currentScreen);
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

            Font    font = GetFontDefault();
            Vector2 pos  = Utils::CenterText(text, font, fontSize, 1.f,
                                             {GetScreenWidth() / 2.f, GetScreenHeight() / 2.f});

            DrawTextEx(font, text, pos, fontSize, 1.f, WHITE);
            break;
        }
        }

        if (inDebugMode) {
            DrawFPS(0, 0);
        }

        EndDrawing();
    }

    // Cleanup

    int retCode = 0;

    CloseWindow();
    Pipes::StopAll();
    if (!Settings::SaveSettings()) {
        ErrorPrintln("Failed to save settings.");
        retCode |= 1;
    }

    return retCode;
}
