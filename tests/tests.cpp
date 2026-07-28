#include <string>

#include "TestSuite/TestSuite.h"

#include "Settings.h"

void BoardTests(void);
void GameManagerTests(void);
void MoveGenTests(void);

int main(int argc, char** argv) {
    TestSuite::Setup(argc, argv);

    Settings::LoadSettings();

    BoardTests();
    GameManagerTests();
    MoveGenTests();

    return TestSuite::RunTests();
}

