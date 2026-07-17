#include <string>

#include "TestSuite/TestSuite.h"

#include "Settings.h"

void BoardTests(void);
void GameManagerTests(void);
void MoveGenTests(void);

int main(void) {
    Settings::LoadSettings();
    
    BoardTests();
    GameManagerTests();
    MoveGenTests();

    TestSuite::RunTests();

    return 0;
}

