#include "Settings.h"

#include <array>
#include <cstring>
#include <fstream>
#include <sstream>
#include <iostream>
#include <print>
#include <utility>

#include "Convert.h"
#include "Utils.h"

constexpr const char* SETTINGS_FILE = "./settings.txt";
constexpr char SETTINGS_DELIM       = ':';

enum class ActualType {
    U8,
    U32,
    U64,
    FLOAT,
    DOUBLE,
    STRING,
};

typedef struct ManyType {
    u8 b  =  UINT8_MAX;
    u32 i = UINT32_MAX;
    u64 l = UINT64_MAX;
    float f  = FLT_MAX;
    double d = DBL_MAX;
    std::string s = "";
} ManyType;

static std::array<std::pair<ActualType, ManyType>, (u64)Setting::TOTAL_SETTINGS> s_settingData;

static Setting DetermineSetting(const std::string& key);
static void SetSetting(Setting setting, ActualType type, const std::string& value);



// ----- Creation / Destruction -----

static void DefaultSettings()
{
    if (s_settingData.size() != (u64)Setting::TOTAL_SETTINGS) {
        ErrorPrintln("Settings::DefaultSettings: Settings data not all initialized to default values");
        exit(1);
    }
    
    for (u64 i = 0; i < (u64)Setting::TOTAL_SETTINGS; i++) {
        // This is used so the compiler warns about not all paths being implemented
        switch ((Setting)i) {
        case Setting::GAME_LOAD:
            s_settingData[i] = std::pair<ActualType, ManyType>{ActualType::U8, {.b = true}};
            break;
        case Setting::GAME_FEN:
            s_settingData[i] = std::pair<ActualType, ManyType>{ActualType::STRING, {.s = DEFAULT_FEN.data()}};
            break;
        case Setting::BOARD_TILE_DARK: {
            Color dark = {100, 75, 60, 255};
            u32 val = Convert::ColorToU32(dark);
            s_settingData[i] = std::pair<ActualType, ManyType>{ActualType::U32, {.i = val}};
            break;
        }
        case Setting::BOARD_TILE_LIGHT: {
            Color light = {175, 150, 120, 255};
            u32 val = Convert::ColorToU32(light);
            s_settingData[i] = std::pair<ActualType, ManyType>{ActualType::U32, {.i = val}};
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
        ActualType type = (s_settingData[(u64)setting]).first;
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
        switch (std::get<0>(item)) {
        case ActualType::U8:
            file << (u16)std::get<1>(item).b;
            break;
        case ActualType::U32:
            file << std::get<1>(item).i;
            break;
        case ActualType::U64:
            file << std::get<1>(item).l;
            break;
        case ActualType::FLOAT:
            file << std::get<1>(item).f;
            break;
        case ActualType::DOUBLE:
            file << std::get<1>(item).d;
            break;
        case ActualType::STRING:
            file << std::get<1>(item).s;
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
    return (s_settingData[(u64)setting]).second.b;
}

u32 Settings::i(Setting setting)
{
    return (s_settingData[(u64)setting]).second.i;
}

u64 Settings::l(Setting setting)
{
    return (s_settingData[(u64)setting]).second.l;
}

float Settings::f(Setting setting)
{
    return (s_settingData[(u64)setting]).second.f;
}

double Settings::d(Setting setting)
{
    return (s_settingData[(u64)setting]).second.d;
}

std::string Settings::s(Setting setting)
{
    return (s_settingData[(u64)setting]).second.s;
}

static Setting DetermineSetting(const std::string& key)
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
    u64 index = static_cast<u64>(setting);
    if (index >= s_settingData.size() || (s_settingData[index]).first != ActualType::U8) {
        return false;
    }
    
    (s_settingData[index]).second.b = value;
    return true;
}

bool Settings::i(Setting setting, u32 value)
{
    u64 index = static_cast<u64>(setting);
    if (index >= s_settingData.size() || (s_settingData[index]).first != ActualType::U32) {
        return false;
    }
    
    (s_settingData[index]).second.i = value;
    return true;
}

bool Settings::l(Setting setting, u64 value)
{
    u64 index = static_cast<u64>(setting);
    if (index >= s_settingData.size() || (s_settingData[index]).first != ActualType::U64) {
        return false;
    }
    
    (s_settingData[index]).second.l = value;
    return true;
}

bool Settings::f(Setting setting, float value)
{
    u64 index = static_cast<u64>(setting);
    if (index >= s_settingData.size() || (s_settingData[index]).first != ActualType::FLOAT) {
        return false;
    }
    
    (s_settingData[index]).second.f = value;
    return true;
}

bool Settings::d(Setting setting, double value)
{
    u64 index = static_cast<u64>(setting);
    if (index >= s_settingData.size() || (s_settingData[index]).first != ActualType::DOUBLE) {
        return false;
    }
    
    (s_settingData[index]).second.d = value;
    return true;
}

bool Settings::s(Setting setting, const std::string& value)
{
    u64 index = static_cast<u64>(setting);
    if (index >= s_settingData.size() || (s_settingData[index]).first != ActualType::STRING) {
        return false;
    }
    
    (s_settingData[index]).second.s = std::string(value);
    return true;
}

static void SetSetting(Setting setting, ActualType type, const std::string& value)
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

