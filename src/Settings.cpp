#include "Settings.h"

#include <cstring>
#include <fstream>
#include <sstream>
#include <iostream>
#include <print>
#include <tuple>
#include <vector>

#include "raylib.h"

#include "Convert.h"
#include "Utils.h"

constexpr const char* SETTINGS_FILE = "./settings.txt";
constexpr char SETTINGS_DELIM       = ':';

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

static Setting DetermineSetting(std::string_view key);
static void SetSetting(Setting setting, ActualType type, std::string_view value);



// ----- Creation / Destruction -----

static void DefaultSettings()
{
    s_settingData.resize((u64)Setting::TOTAL_SETTINGS);

    if (s_settingData.size() != (u64)Setting::TOTAL_SETTINGS) {
        ErrorPrintln("Settings::DefaultSettings: Settings data not all initialized to default values");
        exit(1);
    }
    for (u64 i = 0; i < (u64)Setting::TOTAL_SETTINGS; i++) {
        // This is used so the compiler warns about not all paths being implemented
        switch ((Setting)i) {
        case Setting::GAME_LOAD:
            s_settingData[i] = std::tuple<ManyType, ActualType>{{.b = true}, ActualType::U8};
            break;
        case Setting::GAME_FEN:
            s_settingData[i] = std::tuple<ManyType, ActualType>{{.s = DEFAULT_FEN.data()}, ActualType::STRING};
            break;
        case Setting::BOARD_TILE_DARK: {
            Color dark = {100, 75, 60, 255};
            u32 val = Convert::ColorToU32(dark);
            s_settingData[i] = std::tuple<ManyType, ActualType>{{.i = val}, ActualType::U32};
            break;
        }
        case Setting::BOARD_TILE_LIGHT: {
            Color light = {175, 150, 120, 255};
            u32 val = Convert::ColorToU32(light);
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

    std::ifstream file(SETTINGS_FILE);
    if (!file.is_open()) {
        ErrorPrintln("Settings::LoadSettings: Failed to load settings from file.");
        return false;
    }

    std::string line, token, key, value;
    while (getline(file, line)) {
        std::istringstream ss(line);
        if (!getline(ss, token, SETTINGS_DELIM)) {
            WarningPrintln("Settings::LoadSettings: Could not get key from file");
            continue;
        }
        key = token;

        if (!getline(ss, token)) {
            WarningPrintln("Settings::LoadSettings: Could not get value from file for key: {}", key);
            continue;
        }
        value = token;

        Setting setting = DetermineSetting(key);
        ActualType type = std::get<1>(s_settingData[(u64)setting]);
        SetSetting(setting, type, value);
    }

    file.close();
    return true;
}

bool Settings::SaveSettings()
{
    std::ofstream file(SETTINGS_FILE);
    if (!file.is_open()) {
        ErrorPrintln("Settings::SaveSettings: Could not open file to save settings.");
        return false;
    }
    DebugPrintln("Settings::SaveSettings: Saving settings.");
    
    for (size_t i = 0; i < s_settingData.size(); i++) {
        file << Enums::ToString::Setting[i] << SETTINGS_DELIM;

        const auto& item = s_settingData[i];
        switch (std::get<1>(item)) {
        case ActualType::U8:
            file << (u16)std::get<0>(item).b;
            break;
        case ActualType::U32:
            file << std::get<0>(item).i;
            break;
        case ActualType::U64:
            file << std::get<0>(item).l;
            break;
        case ActualType::FLOAT:
            file << std::get<0>(item).f;
            break;
        case ActualType::DOUBLE:
            file << std::get<0>(item).d;
            break;
        case ActualType::STRING:
            file << std::get<0>(item).s;
            break;
        }

        file << "\n";
    }
    
    DebugPrintln("Settings::SaveSettings: Saved settings.");
    file.close();
    return true;
}



// ----- Read -----

u8 Settings::b(Setting setting)
{
    return std::get<0>(s_settingData[(u64)setting]).b;
}

u32 Settings::i(Setting setting)
{
    return std::get<0>(s_settingData[(u64)setting]).i;
}

u64 Settings::l(Setting setting)
{
    return std::get<0>(s_settingData[(u64)setting]).l;
}

float Settings::f(Setting setting)
{
    return std::get<0>(s_settingData[(u64)setting]).f;
}

double Settings::d(Setting setting)
{
    return std::get<0>(s_settingData[(u64)setting]).d;
}

std::string Settings::s(Setting setting)
{
    return std::get<0>(s_settingData[(u64)setting]).s;
}

static Setting DetermineSetting(std::string_view key)
{
    for (int i = 0; i < (int)Setting::TOTAL_SETTINGS; i++) {
        if (key == Enums::ToString::Setting[i]) {
            return static_cast<Setting>(i);
        }
    }
    
    return Setting::TOTAL_SETTINGS;
}



// ----- Update -----

bool Settings::b(Setting setting, u8 value)
{
    if (std::get<1>(s_settingData[(u64)setting]) != ActualType::U8) {
        return false;
    }
    
    std::get<0>(s_settingData[(u64)setting]).b = value;
    return true;
}

bool Settings::i(Setting setting, u32 value)
{
    if (std::get<1>(s_settingData[(u64)setting]) != ActualType::U32) {
        return false;
    }
    
    std::get<0>(s_settingData[(u64)setting]).i = value;
    return true;
}

bool Settings::l(Setting setting, u64 value)
{
    if (std::get<1>(s_settingData[(u64)setting]) != ActualType::U64) {
        return false;
    }
    
    std::get<0>(s_settingData[(u64)setting]).l = value;
    return true;
}

bool Settings::f(Setting setting, float value)
{
    if (std::get<1>(s_settingData[(u64)setting]) != ActualType::FLOAT) {
        return false;
    }
    
    std::get<0>(s_settingData[(u64)setting]).f = value;
    return true;
}

bool Settings::d(Setting setting, double value)
{
    if (std::get<1>(s_settingData[(u64)setting]) != ActualType::DOUBLE) {
        return false;
    }
    
    std::get<0>(s_settingData[(u64)setting]).d = value;
    return true;
}

bool Settings::s(Setting setting, std::string_view value)
{
    if (std::get<1>(s_settingData[(u64)setting]) != ActualType::STRING) {
        return false;
    }

    std::get<0>(s_settingData[(u64)setting]).s = value.data();
    return true;
}

static void SetSetting(Setting setting, ActualType type, std::string_view value)
{
    switch(type) {
    case ActualType::U8:
        Settings::b(setting, (u8)std::stoul(value.data()));
        break;
    case ActualType::U32:
        Settings::i(setting, std::stoul(value.data()));
        break;
    case ActualType::U64:
        Settings::l(setting, std::stoull(value.data()));
        break;
    case ActualType::FLOAT:
        Settings::f(setting, std::stof(value.data()));
        break;
    case ActualType::DOUBLE:
        Settings::d(setting, std::stod(value.data()));
        break;
    case ActualType::STRING:
        Settings::s(setting, value);
        break;
    }
}

