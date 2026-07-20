#include <string>

#include "TestSuite/TestSuite.h"

#include "Settings.h"

void BoardTests(void);
void GameManagerTests(void);
void MoveGenTests(void);

int main(int argc, char** argv) {
    Settings::LoadSettings();
    
    BoardTests();
    GameManagerTests();
    MoveGenTests();

    TestSuite::RunTests(argc, argv);

    return 0;
}

