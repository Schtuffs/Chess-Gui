#include "Settings.h"

#include <cstring>
#include <print>
#include <tuple>
#include <vector>

#include "raylib.h"

#include "Convert.h"
#include "Utils.h"

typedef struct ManyType {
    u8 b  =  UINT8_MAX;
    u32 i = UINT32_MAX;
    u64 l = UINT64_MAX;
    float f  = FLT_MAX;
    double d = DBL_MAX;
    std::string s = "";
} ManyType;

enum class ActualType {
    U8,
    U32,
    U64,
    FLOAT,
    DOUBLE,
    STRING,
};

static std::vector<std::tuple<ManyType, ActualType>> s_settingData;



// ----- Creation / Destruction -----

static void DefaultSettings()
{
    s_settingData.resize((u64)Setting::TOTAL_SETTINGS);

    if (s_settingData.size() != (u64)Setting::TOTAL_SETTINGS) {
        ErrorPrintln("Settings data not all initialized to default values");
        exit(1);
    }
    for (u64 i = 0; i < (u64)Setting::TOTAL_SETTINGS; i++) {
        // This is used so the compiler warns about not all paths being implemented
        switch ((Setting)i) {
        case Setting::SAVE_GAME_STATE:
            s_settingData[i] = std::tuple<ManyType, ActualType>{{.b = true}, ActualType::U8};
            break;
        case Setting::GAME_STATE:
            s_settingData[i] = std::tuple<ManyType, ActualType>{{.s = DEFAULT_FEN.data()}, ActualType::STRING};
            break;
        case Setting::BOARD_TILE_DARK: {
            Color dark = {100, 75, 60, 255};
            u32 val;
            val = Convert::ColorToU32(dark);
            s_settingData[i] = std::tuple<ManyType, ActualType>{{.i = val}, ActualType::U32};
            break;
        }
        case Setting::BOARD_TILE_LIGHT: {
            Color light = {175, 150, 120, 255};
            u32 val;
            val = Convert::ColorToU32(light);
            s_settingData[i] = std::tuple<ManyType, ActualType>{{.i = val}, ActualType::U32};
            break;
        }
        case Setting::TOTAL_SETTINGS:
            break;
        }
    }
}

bool Settings::LoadSettings()
{
    DefaultSettings();
    // Settings::b(Setting::SAVE_GAME_STATE);
    // Settings::s(Setting::GAME_STATE, DEFAULT_FEN);
    return true;
}

bool Settings::SaveSettings()
{
    return true;
}



// ----- Read -----

u32 Settings::i(Setting setting)
{
    return std::get<0>(s_settingData[(u64)setting]).i;
}

std::string Settings::s(Setting setting)
{
    return std::get<0>(s_settingData[(u64)setting]).s;
}



// ----- Update -----

bool Settings::i(Setting setting, u32 value)
{
    std::get<0>(s_settingData[(u64)setting]).i = value;
    return true;
}

bool Settings::s(Setting setting, std::string_view value)
{
    std::get<0>(s_settingData[(u64)setting]).s = value.data();
    return true;
}

