#include <print>

#include "TestSuite/TestSuite.h"

#include "Board.h"
#include "Convert.h"
#include "MoveGen.h"

void MoveGenTests()
{
    TEST("MoveGen::Generate: london piece movements - king", [](){
        BitBoard expected = 0x00'00'00'00'00'00'18'70;
        Board b("r1bq1rk1/ppp2ppp/2n1pn2/b2pN3/3P1B2/2PBP3/PP3PPP/RN1QK2R w KQ - 3 7");
        
        MoveGen gen;
        BitBoard actual = gen.Generate(b.Pieces(), 4, b.Castling(b.Player()));

        TestSuite::assertEqual(expected, actual);
    });
}

