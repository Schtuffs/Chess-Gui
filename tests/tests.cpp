#include <string>

#include "TestSuite/TestSuite.h"

#include "Utils/Settings.h"
#include "Utils/Utils.h"

void GameManagerTests(void);
void MoveGenTests(void);

static void SetupSettings()
{
    Settings::LoadSettings();

    // Defaults for properly running tests
    Settings::b(Setting::ENGINE_BLACK_AI, false);
    Settings::b(Setting::ENGINE_WHITE_AI, false);
}

int main(int argc, char** argv)
{
    TestSuite::Setup(argc, argv);
    Utils::SetLogLevel(Utils::LogLevel::PRINT);

    SetupSettings();

    GameManagerTests();
    MoveGenTests();

    return TestSuite::RunTests();
}
