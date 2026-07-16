#include <string>

#include "TestSuite.h"

#include "Board.h"

int main(void) {
    TEST("Board move e2e4", [](){
        Board b(DEFAULT_FEN);
        std::string expected = "rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 1 1";

        TestSuite::assertTrue(b.MakeMove("e2e4"));
        std::string actual = b.Fen().data();
        
        TestSuite::assertEqual(expected, actual);
    });
    
    return 0;
}

