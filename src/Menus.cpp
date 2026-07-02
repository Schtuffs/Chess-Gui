#include "Menus.h"

#include <array>
#include <cstdlib>
#include <tuple>
#include <utility>

#include "raygui.h"

#include "GameManager.h"
#include "Renderer.h"
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
    board.RenderBoard(BOARD_SQUARE_DARK, BOARD_SQUARE_LIGHT);
    Rectangle startPos = Utils::StartButtonPos(1, 1, 6, 1);
    
    PushDefaultGuiStyle();
    
    if (Utils::ClickableButton(startPos, "Start new game", 1)) { screen = Enums::Screen::NewGame; }
    if (Utils::ClickableButton(MoveDown(startPos, 4), "Settings", 2)) { screen = Enums::Screen::Settings; }
    if (Utils::ClickableButton(MoveDown(startPos, 1), "Quit", 3)) { screen = Enums::Screen::Quit; }
    
    PopDefaultGuiStyle();
}

void Menu::NewGame(Enums::Screen& screen)
{
    board.RenderBoard(BOARD_SQUARE_DARK, BOARD_SQUARE_LIGHT);
    Rectangle startPos = Utils::StartButtonPos(1, 1, 6, 1);
    
    PushDefaultGuiStyle();
    
    if (Utils::ClickableButton(startPos, "New game", 1)) { screen = Enums::Screen::Game; }
    if (Utils::ClickableButton(MoveDown(startPos, 5), "Back", 2)) { screen = Enums::Screen::Menu; }
    
    PopDefaultGuiStyle();
}

void Menu::Settings(Enums::Screen& screen)
{
    board.RenderBoard(BOARD_SQUARE_DARK, BOARD_SQUARE_LIGHT);
    Rectangle startPos = Utils::StartButtonPos(1, 6, 6, 1);
    
    PushDefaultGuiStyle();
    
    if (Utils::ClickableButton(startPos, "Return", 1)) { screen = Enums::Screen::Menu; }
    
    PopDefaultGuiStyle();
}

void Menu::InGame(Enums::Screen& screen)
{
    (void)screen;
    static GameManager gameManager;
    gameManager.Render();
}

