#include "TestSuite/TestSuite.h"

#include "Board.h"

void BoardTests(void)
{
    TEST("Board::MakeMove: e2e4", [](){
        Board b(DEFAULT_FEN);
        std::string expected = "rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1";

        TestSuite::assertTrue(b.MakeMove("e2e4"));
        std::string actual = b.Fen().data();
        
        TestSuite::assertEqual(expected, actual);
    });

    TEST("Board::MakeMove: opening London", [](){
        Board b(DEFAULT_FEN);
        std::string expected = "r1bqk2r/ppp2ppp/2n1pn2/b2pN3/3P1B2/2P1P3/PP3PPP/RN1QKB1R w KQkq - 1 6";
        
        std::vector<std::string> moves = {"d2d4", "d7d5", "g1f3", "g8f6", "c1f4", "b8c6", "e2e3", "e7e6", "f3e5", "f8b4", "c2c3", "b4a5"};
        for (const auto& move : moves) {
            TestSuite::assertTrue(b.MakeMove(move));
        }
        std::string actual = b.Fen().data();
        
        TestSuite::assertEqual(expected, actual);
    });
    
    TEST("Board::MakeMove opening Kings Indian", [](){
        Board b(DEFAULT_FEN);
        std::string expected = "rnbq1rk1/ppp2pbp/3p1np1/4p3/2PPP3/2N2N2/PP2BPPP/R1BQK2R w KQ e6 0 6";
        
        std::vector<std::string> moves = {"d2d4", "g8f6", "c2c4", "g7g6", "b1c3", "f8g7", "e2e4", "d7d6", "g1f3", "O-O", "f1e2", "e7e5"};
        for (const auto& move : moves) {
            TestSuite::assertTrue(b.MakeMove(move));
        }
        std::string actual = b.Fen().data();
        
        TestSuite::assertEqual(expected, actual);
    });
}

