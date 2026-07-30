#include <string>

#include "TestSuite/TestSuite.h"

#include "Settings.h"
#include "Utils.h"

void BoardTests(void);
void GameManagerTests(void);
void MoveGenTests(void);

int main(int argc, char** argv) {
    TestSuite::Setup(argc, argv);
    Utils::SetLogLevel(Utils::LogLevel::PRINT);

    Settings::LoadSettings();

    BoardTests();
    GameManagerTests();
    MoveGenTests();

    return TestSuite::RunTests();
}

