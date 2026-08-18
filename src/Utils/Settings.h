#pragma once

#include <cfloat>
#include <string>

#include "Utils/Constants.h"

/**
 * @brief A specific settings.
 * @enum Setting
 * @date 2026-07-01
 */
enum class Setting {
    GAME_LOAD,
    GAME_FEN,
    GAME_MOVES,
    ENGINE_WHITE_AI,
    ENGINE_WHITE_PATH,
    ENGINE_BLACK_AI,
    ENGINE_BLACK_PATH,
    BOARD_TILE_DARK,
    BOARD_TILE_LIGHT,
    BOARD_TILE_PROMO,
    BOARD_TILE_LEGAL,
    TOTAL_SETTINGS
};

/**
 * @brief Stores settings about the game.
 * @namespace Settings
 * @date 2026-07-01
 */
namespace Settings {

// ----- Creation / Destruction -----

/**
 * @brief Saves `Settings` to a file.
 * @date 2026-07-01
 */
bool SaveSettings();

/**
 * @brief Load `Settings` from a file.
 * @date 2026-07-01
 */
bool LoadSettings();

// Resets all `Settings`.
void Reset();

// ----- Read -----

/**
 * @brief Get a u8/bool `Setting`.
 * @param setting The `Setting` value to get.
 * @return The `Setting` value, or UINT32_MAX on fail.
 * @date 2026-07-01
 */
u8 b(Setting setting);

/**
 * @brief Get a u32 `Setting`.
 * @param setting The `Setting` value to get.
 * @return The `Setting` value, or UINT32_MAX on fail.
 * @date 2026-07-01
 */
u32 i(Setting setting);

/**
 * @brief Get a u64 `Setting`.
 * @param setting The `Setting` value to get.
 * @return The `Setting` value, or UINT64_MAX on fail.
 * @date 2026-07-01
 */
u64 l(Setting setting);

/**
 * @brief Get a float `Setting`.
 * @param setting The `Setting` value to get.
 * @return The `Setting` value, or FLT_MAX on fail.
 * @date 2026-07-01
 */
float f(Setting setting);

/**
 * @brief Get a double `Setting`.
 * @param setting The `Setting` value to get.
 * @return The `Setting` value, or DLB_MAX on fail.
 * @date 2026-07-01
 */
double d(Setting setting);

/**
 * @brief Get a string `Setting`.
 * @param setting The `Setting` value to get.
 * @return The `Setting` value, or empty string on fail.
 * @date 2026-07-01
 */
std::string s(Setting setting);

// ----- Update -----

/**
 * @brief Set a u8 `Setting`.
 * @param setting The `Setting` value to set.
 * @param value The value to put into the `Setting`.
 * @return `true` on success and valid type.
 * @date 2026-07-01
 */
bool b(Setting setting, u8 value);

/**
 * @brief Set a u32 `Setting`.
 * @param setting The `Setting` value to set.
 * @param value The value to put into the `Setting`.
 * @return `true` on success and valid type.
 * @date 2026-07-01
 */
bool i(Setting setting, u32 value);

/**
 * @brief Set a u64 `Setting`.
 * @param setting The `Setting` value to set.
 * @param value The value to put into the `Setting`.
 * @return `true` on success and valid type.
 * @date 2026-07-01
 */
bool l(Setting setting, u64 value);

/**
 * @brief Set a float `Setting`.
 * @param setting The `Setting` value to set.
 * @param value The value to put into the `Setting`.
 * @return `true` on success and valid type.
 * @date 2026-07-01
 */
bool f(Setting setting, float value);

/**
 * @brief Set a double `Setting`.
 * @param setting The `Setting` value to set.
 * @param value The value to put into the `Setting`.
 * @return `true` on success and valid type.
 * @date 2026-07-01
 */
bool d(Setting setting, double value);

/**
 * @brief Set a string `Setting`.
 * @param setting The `Setting` value to set.
 * @param value The value to put into the `Setting`.
 * @return `true` on success and valid type.
 * @date 2026-07-01
 */
bool s(Setting setting, const std::string& value);
} // namespace Settings
