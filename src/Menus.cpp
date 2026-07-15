#include "Menus.h"

#include <array>
#include <cstdlib>
#include <tuple>
#include <utility>

#include "raygui.h"

#include "Convert.h"
#include "GameManager.h"
#include "Renderer.h"
#include "Settings.h"
#include "Utils.h"

static Renderer board;

std::array defaultGuiStyle {
    std::tuple { DEFAULT, (int)TEXT_SIZE, 0 },
    std::tuple { DEFAULT, (int)BACKGROUND_COLOR, 0 },
};

static void PushDefaultGuiStyle()
{
    for (size_t i = 0; i < defaultGuiStyle.size(); i++) {
        std::get<2>(defaultGuiStyle[i]) = GuiGetStyle(std::get<0>(defaultGuiStyle[i]), std::get<1>(defaultGuiStyle[i]));
    }
    GuiSetStyle(DEFAULT, TEXT_SIZE, Utils::Max(Utils::Min(GetScreenWidth(), GetScreenHeight()) / 50, 10));
}

static void PopDefaultGuiStyle()
{
    for (size_t i = 0; i < defaultGuiStyle.size(); i++) {
        GuiSetStyle(std::get<0>(defaultGuiStyle[i]), std::get<1>(defaultGuiStyle[i]), std::get<2>(defaultGuiStyle[i]));
    }
}

static Rectangle& MoveDown(Rectangle& rect, u8 squares)
{
    Vector3 grid = Utils::GridPositioning();
    rect.y += grid.z * squares;
    return rect;
}

void Menu::Main(Enums::Screen& screen)
{
    Color dark  = Convert::U32ToColor(Settings::i(Setting::BOARD_TILE_DARK));
    Color light = Convert::U32ToColor(Settings::i(Setting::BOARD_TILE_LIGHT));
    
    board.FixSize();
    board.RenderBoard(dark, light);
    Rectangle startPos = Utils::ButtonPos(1, 1, 6, 1);
    
    PushDefaultGuiStyle();
    
    if (Utils::ClickableButton(startPos, "Start new game", 1)) { screen = Enums::Screen::NewGame; }
    if (Utils::ClickableButton(MoveDown(startPos, 4), "Settings", 2)) { screen = Enums::Screen::Settings; }
    if (Utils::ClickableButton(MoveDown(startPos, 1), "Quit", 3)) { screen = Enums::Screen::Quit; }
    
    PopDefaultGuiStyle();
}

void Menu::NewGame(Enums::Screen& screen)
{
    Color dark  = Convert::U32ToColor(Settings::i(Setting::BOARD_TILE_DARK));
    Color light = Convert::U32ToColor(Settings::i(Setting::BOARD_TILE_LIGHT));

    board.FixSize();
    board.RenderBoard(dark, light);
    Rectangle startPos = Utils::ButtonPos(1, 1, 6, 1);
    
    PushDefaultGuiStyle();
    
    int id = 1;
    if (Utils::ClickableButton(startPos, "New game", id++)) {
        screen = Enums::Screen::Game;
        Settings::b(Setting::GAME_LOAD, 1);
    }
    if (Utils::ClickableButton(MoveDown(startPos, 1), "Load game", id++)) {
        screen = Enums::Screen::Game;
        Settings::b(Setting::GAME_LOAD, 2);
    }
    if (Utils::ClickableButton(MoveDown(startPos, 4), "Back", id++)) { screen = Enums::Screen::Menu; }
    
    PopDefaultGuiStyle();
}

void Menu::Settings(Enums::Screen& screen)
{
    static bool settingsLoaded = false;
    static Vector3 darkHSV, lightHSV;
    if (!settingsLoaded) {
        settingsLoaded = true;

        Color dark  = Convert::U32ToColor(Settings::i(Setting::BOARD_TILE_DARK));
        Color light = Convert::U32ToColor(Settings::i(Setting::BOARD_TILE_LIGHT));

        darkHSV  = ColorToHSV(dark);
        lightHSV = ColorToHSV(light);
    }
    
    float barSize = GuiGetStyle(COLORPICKER, HUEBAR_WIDTH) + GuiGetStyle(COLORPICKER, HUEBAR_PADDING);
    Rectangle  darkPicker = Utils::ButtonPos(1, 1, 2, 2);
    Rectangle lightPicker = Utils::ButtonPos(5, 1, 2, 2);
    darkPicker.width -= barSize;
    lightPicker.width -= barSize;
    
    board.FixSize();
    board.RenderBoard(ColorFromHSV(darkHSV.x, darkHSV.y, darkHSV.z), ColorFromHSV(lightHSV.x, lightHSV.y, lightHSV.z));
    
    PushDefaultGuiStyle();
    
    GuiColorPickerHSV(darkPicker,  nullptr, &darkHSV);
    GuiColorPickerHSV(lightPicker, nullptr, &lightHSV);

    Rectangle saveButton = Utils::ButtonPos(1, 6, 3, 1);
    if (Utils::ClickableButton(saveButton, "Save", 1)) {
        DebugPrintln("Menu::Settings: Saving settings.");

        Settings::i(Setting::BOARD_TILE_DARK,  Convert::ColorToU32(ColorFromHSV(darkHSV.x, darkHSV.y, darkHSV.z)));
        Settings::i(Setting::BOARD_TILE_LIGHT, Convert::ColorToU32(ColorFromHSV(lightHSV.x, lightHSV.y, lightHSV.z)));
        
        DebugPrintln("Menu::Settings: Saved settings.");
    }

    Rectangle returnButton = Utils::ButtonPos(4, 6, 3, 1);
    if (Utils::ClickableButton(returnButton, "Return", 2)) { settingsLoaded = false; screen = Enums::Screen::Menu; }
    
    PopDefaultGuiStyle();
}

void Menu::InGame(Enums::Screen& screen)
{
    (void)screen;
    static bool isWhitePerspective = true;
    static GameManager* gameManager = new GameManager(DEFAULT_FEN);

    u8 load = Settings::b(Setting::GAME_LOAD);
    if (load > 0) {
        Settings::b(Setting::GAME_LOAD, 0);
        delete gameManager;
        if (load == 1) {
            gameManager = new GameManager(DEFAULT_FEN);
        }
        else {
            gameManager = new GameManager(Settings::s(Setting::GAME_FEN));
        }
        screen = Enums::Screen::Game;
        board.RenderMoves(0, true);
    }
    
    if (IsKeyPressed(KEY_F)) {
        isWhitePerspective = !isWhitePerspective;
    }
    
    Color dark = Convert::U32ToColor(Settings::i(Setting::BOARD_TILE_DARK));
    Color light = Convert::U32ToColor(Settings::i(Setting::BOARD_TILE_LIGHT));
    
    std::string move = board.CheckMove(isWhitePerspective);
    board.FixSize();
    gameManager->Update(move);
    board.RenderBoard(dark, light);
    board.RenderMoves(gameManager->Moves(), isWhitePerspective);
    board.RenderPieces(gameManager->Fen(), isWhitePerspective);
}

