#pragma once

#include "Constants.h"

/**
 * @brief All of the displayable screens within the program.
 * @namespace Menu
 * @date 2026-06-28
 */
namespace Menu
{
    /**
     * @brief The main menu.
     * @param screen The screen state which can be changed by this `Menu`.
     * @date 2026-06-28
     */
    void Main(Enums::Screen& screen);

    /**
     * @brief The new game menu.
     * @param screen The screen state which can be changed by this `Menu`.
     * @date 2026-06-28
     */
    void NewGame(Enums::Screen& screen);

    /**
     * @brief The settings menu.
     * @param screen The screen state which can be changed by this `Menu`.
     * @date 2026-06-28
     */
    void Settings(Enums::Screen& screen);

    /**
     * @brief The in game screen.
     * @param screen The screen state which can be changed by this `Menu`.
     * @date 2026-06-28
     */
    void InGame(Enums::Screen& screen);
}


