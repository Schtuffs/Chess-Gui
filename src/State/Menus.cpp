#include "Menus.h"

#include <array>
#include <cstdlib>
#include <cstring>
#include <tuple>
#include <utility>

#include "raygui.h"
#undef WHITE
#undef BLACK

#include "State/GameManager.h"
#include "Utils/Convert.h"
#include "Utils/Fen.h"
#include "Utils/Renderer.h"
#include "Utils/Settings.h"
#include "Utils/Utils.h"

constexpr u8          LOAD_DEFAULT_FEN = 1;
constexpr u8          LOAD_SAVED_FEN   = 2;
constexpr const char* BACK_MESSAGE     = "Return";
static Renderer       renderer;
float                 DefaultButtonThickness();
Color                 DefaultButtonBorderColour();

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
                std::max(std::min(GetScreenWidth(), GetScreenHeight()) / 50, 10));
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
    renderer.Render("", 0ull, SQ_BAD, true);
    Rectangle startPos = Utils::ButtonPos(1, 1, 6, 1);

    PushDefaultGuiStyle();

    if (Utils::ClickableButton(startPos, "Start game", 1)) {
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
    renderer.Render("", 0ull, SQ_BAD, true);

    PushDefaultGuiStyle();

    int id = 1;
    if (Utils::ClickableButton(Utils::ButtonPos(1, 1, 6, 1), "New game", id++)) {
        screen = Enums::Screen::Game;
        Settings::b(Setting::GAME_LOAD, LOAD_DEFAULT_FEN);
    }
    if (Utils::ClickableButton(Utils::ButtonPos(1, 2, 6, 1), "Load game", id++)) {
        screen = Enums::Screen::Game;
        Settings::b(Setting::GAME_LOAD, LOAD_SAVED_FEN);
    }
    if (Utils::ClickableButton(Utils::ButtonPos(1, 3, 6, 1), "Custom game", id++)) {
        screen = Enums::Screen::GameCreation;
    }
    if (Utils::ClickableButton(Utils::ButtonPos(1, 6, 6, 1), BACK_MESSAGE, id++)) {
        screen = Enums::Screen::Menu;
    }
    if (IsKeyPressed(KEY_ESCAPE)) {
        screen = Enums::Screen::Menu;
    }

    PopDefaultGuiStyle();
}

void Menu::CreateGame(Enums::Screen& screen)
{
    PushDefaultGuiStyle();

    constexpr u64 MAX_FEN_SIZE = 200;

    static bool  settingsLoaded = false;
    static bool  isTypingFen    = false;
    static char* fen            = nullptr;
    if (!settingsLoaded) {
        settingsLoaded = true;
        delete[] fen;
        fen = new char[MAX_FEN_SIZE];
        std::strncpy(fen, Fen::DEFAULT.data(), MAX_FEN_SIZE);
    }

    renderer.Update();
    renderer.Render(fen, 0ull, SQ_BAD, true);

    if (GuiTextBox(Utils::ButtonPos(8, 1, 3, 1), fen, MAX_FEN_SIZE, isTypingFen)) {
        isTypingFen = !isTypingFen;
    }

    u8 id = 1;
    if (Utils::ClickableButton(Utils::ButtonPos(8, 6, 3, 1), "Begin with fen", id++)) {
        settingsLoaded = false;
        screen         = Enums::Screen::Game;
        Settings::s(Setting::GAME_FEN, fen);
        Settings::b(Setting::GAME_LOAD, LOAD_SAVED_FEN);
    }
    if (Utils::ClickableButton(Utils::ButtonPos(8, 7, 3, 1), BACK_MESSAGE, id++)) {
        settingsLoaded = false;
        screen         = Enums::Screen::NewGame;
    }

    if (IsKeyPressed(KEY_ESCAPE)) {
        settingsLoaded = false;
        screen         = Enums::Screen::NewGame;
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
    renderer.Render("", 0ull, SQ_BAD, true);

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

    if (Utils::ClickableButton(Utils::ButtonPos(4, 6, 3, 1), BACK_MESSAGE, id++)) {
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
    renderer.Render("", 0ull, SQ_BAD, true);

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
    renderer.Render("", 0ull, SQ_BAD, true);

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
            renderer.RenderSquare(HSVToColor(promoHSV), Square(17 + off), true);
        } else if (i == 2 || i == 3) {
            renderer.RenderSquare(HSVToColor(legalHSV), Square(17 + off), true);
        } else if ((i + (i / 6)) % 2) {
            renderer.RenderSquare(HSVToColor(darkHSV), Square(17 + off), false);
        } else {
            renderer.RenderSquare(HSVToColor(lightHSV), Square(17 + off), false);
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

    if (Utils::ClickableButton(Utils::ButtonPos(4, 6, 3, 1), BACK_MESSAGE, id++)) {
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
    constexpr u64 MAX_PATH_SIZE = 200;
    PushDefaultGuiStyle();

    static bool isWhiteAI      = false;
    static bool isBlackAI      = false;
    static bool settingsLoaded = false;

    static char* whiteEngine = nullptr;
    static bool  whiteTyping = false;
    static char* blackEngine = nullptr;
    static bool  blackTyping = false;
    if (!settingsLoaded) {
        settingsLoaded = true;
        isWhiteAI      = Settings::b(Setting::ENGINE_WHITE_AI);
        isBlackAI      = Settings::b(Setting::ENGINE_BLACK_AI);

        whiteTyping = false;
        delete[] whiteEngine;
        whiteEngine = new char[MAX_PATH_SIZE];
        std::strncpy(whiteEngine, Settings::s(Setting::ENGINE_WHITE_PATH).c_str(), MAX_PATH_SIZE);
        whiteEngine[MAX_PATH_SIZE - 1] = '\0';

        blackTyping = false;
        delete[] blackEngine;
        blackEngine = new char[MAX_PATH_SIZE];
        std::strncpy(blackEngine, Settings::s(Setting::ENGINE_BLACK_PATH).c_str(), MAX_PATH_SIZE);
        blackEngine[MAX_PATH_SIZE - 1] = '\0';
    }

    renderer.Update();
    renderer.Render("", 0ull, SQ_BAD, true);

    SettingScreens screen = SETTING_ENGINE;

    u8 id = 1;
    if (Utils::ClickableButton(Utils::ButtonPos(1, 1, 3, 1),
                               (isWhiteAI ? "White: AI" : "White: Human"), id++)) {
        isWhiteAI = !isWhiteAI;
        Settings::b(Setting::ENGINE_WHITE_AI, isWhiteAI);
    }
    if (GuiTextBox(Utils::ButtonPos(1, 2, 3, 1), whiteEngine, MAX_PATH_SIZE, whiteTyping)) {
        whiteTyping = !whiteTyping;
    }

    if (Utils::ClickableButton(Utils::ButtonPos(4, 1, 3, 1),
                               (isBlackAI ? "Black: AI" : "Black: Human"), id++)) {
        isBlackAI = !isBlackAI;
        Settings::b(Setting::ENGINE_BLACK_AI, isBlackAI);
    }
    if (GuiTextBox(Utils::ButtonPos(4, 2, 3, 1), blackEngine, MAX_PATH_SIZE, blackTyping)) {
        blackTyping = !blackTyping;
    }

    if (Utils::ClickableButton(Utils::ButtonPos(4, 6, 3, 1), BACK_MESSAGE, id++)) {
        screen         = SETTING_MAIN;
        settingsLoaded = false;
        Settings::s(Setting::ENGINE_WHITE_PATH, whiteEngine);
        Settings::s(Setting::ENGINE_BLACK_PATH, blackEngine);
    }

    if (IsKeyPressed(KEY_ESCAPE)) {
        screen         = SETTING_MAIN;
        settingsLoaded = false;
        Settings::s(Setting::ENGINE_WHITE_PATH, whiteEngine);
        Settings::s(Setting::ENGINE_BLACK_PATH, blackEngine);
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

PieceType DeterminePromo(Square sq, Square promo)
{
    constexpr PieceType TYPES[] = {QUEEN, ROOK, BISHOP, KNIGHT};

    if (sq % 8 != promo % 8) {
        return TYPE_NONE;
    }

    i8 off   = i8(promo / 8) - i8(sq / 8);
    u8 piece = std::abs(off);

    return TYPES[piece];
}

void Menu::InGame(Enums::Screen& screen)
{
    static bool         isWhitePerspective = true;
    static GameManager* gameManager        = nullptr;

    u8 load = Settings::b(Setting::GAME_LOAD);
    if (load > 0) {
        Settings::b(Setting::GAME_LOAD, 0);
        delete gameManager;
        if (load == LOAD_SAVED_FEN) {
            gameManager = new GameManager(Settings::s(Setting::GAME_FEN));
        } else {
            gameManager = new GameManager(Fen::DEFAULT);
        }

        screen = Enums::Screen::Game;
        gameManager->IsReady();
        isWhitePerspective = true;
    }

    if (IsKeyPressed(KEY_F)) {
        isWhitePerspective = !isWhitePerspective;
    }
    if (IsKeyPressed(KEY_ESCAPE)) {
        screen = Enums::Screen::Menu;
    }

    renderer.Update();
    Square sq = renderer.Render(gameManager->Fen(), gameManager->Moves(), gameManager->Promotion(),
                                isWhitePerspective);

    if (Utils::IsValidSquare(sq)) {
        DebugPrintln("Menu::InGame: Selected square: {}", (u8)sq);
        if (gameManager->Promotion() != SQ_BAD) {
            // Render the stuff
            PieceType type = DeterminePromo(sq, gameManager->Promotion());
            if (type != TYPE_NONE) {
                gameManager->Promote(type);
            }
        } else if (gameManager->Held() == SQ_BAD) {
            gameManager->Pickup(sq);
        } else {
            gameManager->Update(Move::Make(gameManager->Held(), sq));
        }
    }

    if (gameManager->InCheckmate() || gameManager->InStalemate()) {
        renderer.RenderMate(~(gameManager->Player()), gameManager->InCheckmate());
    }
}
