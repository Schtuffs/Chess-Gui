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

static Renderer renderer;
float           DefaultButtonThickness();
Color           DefaultButtonBorderColour();

std::array defaultGuiStyle{
    std::tuple{DEFAULT, (int)TEXT_SIZE, 0},
    std::tuple{DEFAULT, (int)BACKGROUND_COLOR, 0},
};

static void PushDefaultGuiStyle()
{
    for (size_t i = 0; i < defaultGuiStyle.size(); i++) {
        std::get<2>(defaultGuiStyle[i]) =
            GuiGetStyle(std::get<0>(defaultGuiStyle[i]), std::get<1>(defaultGuiStyle[i]));
    }
    GuiSetStyle(DEFAULT, TEXT_SIZE,
                Utils::Max(Utils::Min(GetScreenWidth(), GetScreenHeight()) / 50, 10));
}

static void PopDefaultGuiStyle()
{
    for (size_t i = 0; i < defaultGuiStyle.size(); i++) {
        GuiSetStyle(std::get<0>(defaultGuiStyle[i]), std::get<1>(defaultGuiStyle[i]),
                    std::get<2>(defaultGuiStyle[i]));
    }
}

static Rectangle MoveDown(Rectangle rect, u8 squares)
{
    Vector3 grid = Utils::GridPositioning();
    rect.y += grid.z * squares;
    return rect;
}

void Menu::Main(Enums::Screen& screen)
{
    renderer.Update();
    renderer.Render("", 0, 64, true);
    Rectangle startPos = Utils::ButtonPos(1, 1, 6, 1);

    PushDefaultGuiStyle();

    if (Utils::ClickableButton(startPos, "Start new game", 1)) {
        screen = Enums::Screen::NewGame;
    }
    if (Utils::ClickableButton((startPos = MoveDown(startPos, 4)), "Settings", 2)) {
        screen = Enums::Screen::Settings;
    }
    if (Utils::ClickableButton((startPos = MoveDown(startPos, 1)), "Quit", 3)) {
        screen = Enums::Screen::Quit;
    }

    PopDefaultGuiStyle();
}

void Menu::NewGame(Enums::Screen& screen)
{
    renderer.Update();
    renderer.Render("", 0, 64, true);
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
    if (Utils::ClickableButton(MoveDown(startPos, 4), "Back", id++)) {
        screen = Enums::Screen::Menu;
    }
    if (IsKeyPressed(KEY_ESCAPE)) {
        screen = Enums::Screen::Menu;
    }

    PopDefaultGuiStyle();
}

Color HSVToColor(Vector3 hsv) { return ColorFromHSV(hsv.x, hsv.y, hsv.z); }

enum SettingScreens {
    SETTING_LEAVE,
    SETTING_MAIN,
    SETTING_RESET,
    SETTING_BOARD,
    SETTING_ENGINE,
};

static SettingScreens SettingsMain()
{
    PushDefaultGuiStyle();

    renderer.Update();
    renderer.Render("", MoveGen::INVALID, 64, true);

    SettingScreens screen = SETTING_MAIN;

    u8 id = 1;
    if (Utils::ClickableButton(Utils::ButtonPos(1, 1, 3, 1), "Board", id++)) {
        screen = SETTING_BOARD;
    }

    if (Utils::ClickableButton(Utils::ButtonPos(4, 1, 3, 1), "Engine", id++)) {
        screen = SETTING_ENGINE;
    }

    if (Utils::ClickableButton(Utils::ButtonPos(1, 6, 3, 1), "Reset", id++)) {
        screen = SETTING_RESET;
    }

    if (Utils::ClickableButton(Utils::ButtonPos(4, 6, 3, 1), "Return", id++)) {
        screen = SETTING_LEAVE;
    }

    if (IsKeyPressed(KEY_ESCAPE)) {
        screen = SETTING_LEAVE;
    }

    PopDefaultGuiStyle();
    return screen;
}

static SettingScreens SettingsReset()
{
    PushDefaultGuiStyle();

    renderer.Update();
    renderer.Render("", MoveGen::INVALID, 64, true);

    SettingScreens screen = SETTING_RESET;

    Utils::ClickableButton(Utils::ButtonPos(1, 2, 6, 1), "Reset all settings to defaults?", 255);

    u8 id = 1;
    if (Utils::ClickableButton(Utils::ButtonPos(1, 4, 3, 1), "Yes", id++)) {
        screen = SETTING_MAIN;
        Settings::Reset();
    }

    if (Utils::ClickableButton(Utils::ButtonPos(4, 4, 3, 1), "No", id++)) {
        screen = SETTING_MAIN;
    }

    if (Utils::ClickableButton(Utils::ButtonPos(4, 6, 3, 1), "Cancel", id++)) {
        screen = SETTING_MAIN;
    }

    if (IsKeyPressed(KEY_ESCAPE)) {
        screen = SETTING_MAIN;
    }

    PopDefaultGuiStyle();
    return screen;
}

static SettingScreens SettingsBoard()
{
    static bool    settingsLoaded = false;
    static Vector3 darkHSV, lightHSV, promoHSV, legalHSV;
    SettingScreens screen = SETTING_BOARD;
    if (!settingsLoaded) {
        settingsLoaded = true;

        Color dark  = Convert::U32ToColor(Settings::i(Setting::BOARD_TILE_DARK));
        Color light = Convert::U32ToColor(Settings::i(Setting::BOARD_TILE_LIGHT));
        Color promo = Convert::U32ToColor(Settings::i(Setting::BOARD_TILE_PROMO));
        Color legal = Convert::U32ToColor(Settings::i(Setting::BOARD_TILE_LEGAL));

        darkHSV  = ColorToHSV(dark);
        lightHSV = ColorToHSV(light);
        promoHSV = ColorToHSV(promo);
        legalHSV = ColorToHSV(legal);
    }
    renderer.Update();
    renderer.Render("", MoveGen::INVALID, 64, true);

    float barSize =
        GuiGetStyle(COLORPICKER, HUEBAR_WIDTH) + GuiGetStyle(COLORPICKER, HUEBAR_PADDING);
    Rectangle darkPicker  = Utils::ButtonPos(0, 1, 2, 2);
    Rectangle lightPicker = Utils::ButtonPos(2, 1, 2, 2);
    Rectangle promoPicker = Utils::ButtonPos(4, 1, 2, 2);
    Rectangle legalPicker = Utils::ButtonPos(6, 1, 2, 2);
    darkPicker.width -= barSize;
    lightPicker.width -= barSize;
    promoPicker.width -= barSize;
    legalPicker.width -= barSize;

    PushDefaultGuiStyle();

    Utils::ClickableButton(Utils::ButtonPos(0, 0, 2, 1), "Dark", 0);
    GuiColorPickerHSV(darkPicker, nullptr, &darkHSV);
    Utils::ClickableButton(Utils::ButtonPos(2, 0, 2, 1), "Light", 0);
    GuiColorPickerHSV(lightPicker, nullptr, &lightHSV);
    Utils::ClickableButton(Utils::ButtonPos(4, 0, 2, 1), "Promo", 0);
    GuiColorPickerHSV(promoPicker, nullptr, &promoHSV);
    Utils::ClickableButton(Utils::ButtonPos(6, 0, 2, 1), "Legal", 0);
    GuiColorPickerHSV(legalPicker, nullptr, &legalHSV);

    for (int i = 0; i < 18; i++) {
        i8 off = ((i / 6) * 8) + ((i % 6));
        if (i == 14 || i == 15) {
            renderer.RenderSquare(HSVToColor(promoHSV), 17 + off, true);
        } else if (i == 2 || i == 3) {
            renderer.RenderSquare(HSVToColor(legalHSV), 17 + off, true);
        } else if ((i + (i / 6)) % 2) {
            renderer.RenderSquare(HSVToColor(darkHSV), 17 + off, false);
        } else {
            renderer.RenderSquare(HSVToColor(lightHSV), 17 + off, false);
        }
    }

    Vector3   grid = Utils::GridPositioning();
    Rectangle rect = {(grid.x + grid.z * 1), (grid.y + grid.z * 3), (grid.z * 6), (grid.z * 3)};
    DrawRectangleLinesEx(rect, DefaultButtonThickness(), DefaultButtonBorderColour());

    u8 id = 1;
    if (Utils::ClickableButton(Utils::ButtonPos(1, 6, 3, 1), "Save", id++)) {
        DebugPrintln("Menu::Settings: Saving settings.");

        Settings::i(Setting::BOARD_TILE_DARK, Convert::ColorToU32(HSVToColor(darkHSV)));
        Settings::i(Setting::BOARD_TILE_LIGHT, Convert::ColorToU32(HSVToColor(lightHSV)));
        Settings::i(Setting::BOARD_TILE_PROMO, Convert::ColorToU32(HSVToColor(promoHSV)));
        Settings::i(Setting::BOARD_TILE_LEGAL, Convert::ColorToU32(HSVToColor(legalHSV)));

        DebugPrintln("Menu::Settings: Saved settings.");
    }

    if (Utils::ClickableButton(Utils::ButtonPos(4, 6, 3, 1), "Return", id++)) {
        settingsLoaded = false;
        screen         = SETTING_MAIN;
    }

    if (IsKeyPressed(KEY_ESCAPE)) {
        screen         = SETTING_MAIN;
        settingsLoaded = false;
    }

    PopDefaultGuiStyle();

    return screen;
}

static SettingScreens SettingsEngine()
{
    PushDefaultGuiStyle();

    static bool isWhiteAI      = false;
    static bool isBlackAI      = false;
    static bool settingsLoaded = false;
    if (!settingsLoaded) {
        settingsLoaded = true;
        isWhiteAI      = Settings::b(Setting::ENGINE_WHITE_AI);
        isBlackAI      = Settings::b(Setting::ENGINE_BLACK_AI);
    }

    renderer.Update();
    renderer.Render("", 0, 64, true);

    SettingScreens screen = SETTING_ENGINE;

    u8 id = 1;
    if (Utils::ClickableButton(Utils::ButtonPos(1, 1, 3, 1),
                               (isWhiteAI ? "White: AI" : "White: Human"), id++)) {
        isWhiteAI = !isWhiteAI;
        Settings::b(Setting::ENGINE_WHITE_AI, isWhiteAI);
    }

    if (Utils::ClickableButton(Utils::ButtonPos(4, 1, 3, 1),
                               (isBlackAI ? "Black: AI" : "Black: Human"), id++)) {
        isBlackAI = !isBlackAI;
        Settings::b(Setting::ENGINE_BLACK_AI, isBlackAI);
    }

    if (Utils::ClickableButton(Utils::ButtonPos(4, 6, 3, 1), "Return", id++)) {
        screen         = SETTING_MAIN;
        settingsLoaded = false;
    }

    if (IsKeyPressed(KEY_ESCAPE)) {
        screen         = SETTING_MAIN;
        settingsLoaded = false;
    }

    PopDefaultGuiStyle();

    return screen;
}

void Menu::Settings(Enums::Screen& screen)
{
    static SettingScreens setting = SETTING_MAIN;

    switch (setting) {
    case SETTING_LEAVE:
        setting = SETTING_MAIN;
        SettingsMain();
        screen = Enums::Screen::Menu;
        break;
    case SETTING_MAIN:
        setting = SettingsMain();
        break;
    case SETTING_RESET:
        setting = SettingsReset();
        break;
    case SETTING_BOARD:
        setting = SettingsBoard();
        break;
    case SETTING_ENGINE:
        setting = SettingsEngine();
        break;
    }

    if (setting == SETTING_LEAVE) {
        setting = SETTING_MAIN;
        screen  = Enums::Screen::Menu;
    }
}

void Menu::InGame(Enums::Screen& screen)
{
    (void)screen;
    static bool         isWhitePerspective = true;
    static GameManager* gameManager        = nullptr;

    u8 load = Settings::b(Setting::GAME_LOAD);
    if (load > 0) {
        Settings::b(Setting::GAME_LOAD, 0);
        delete gameManager;
        if (load == 1) {
            gameManager = new GameManager(DEFAULT_FEN);
        } else {
            gameManager = new GameManager(Settings::s(Setting::GAME_FEN));
        }
        screen = Enums::Screen::Game;
    }

    if (IsKeyPressed(KEY_F)) {
        isWhitePerspective = !isWhitePerspective;
    }
    if (IsKeyPressed(KEY_ESCAPE)) {
        screen = Enums::Screen::Menu;
    }

    renderer.Update();
    std::string move = renderer.Render(gameManager->Fen(), gameManager->Moves(),
                                       gameManager->Promotion(), isWhitePerspective);
    gameManager->Update(move);
    if (gameManager->InCheckmate() || gameManager->InStalemate()) {
        renderer.RenderMate(Utils::SwapColour(gameManager->Player()), gameManager->InCheckmate());
    }
}
