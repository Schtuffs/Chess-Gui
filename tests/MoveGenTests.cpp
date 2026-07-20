#include <print>

#include "TestSuite/TestSuite.h"

#include "Board.h"
#include "Convert.h"
#include "MoveGen.h"

static void WhiteLondonTests()
{
    constexpr const char* fen = "r1bq1rk1/ppp2ppp/2n1pn2/b2pN3/3P1B2/2PBP3/PP3PPP/RN1QK2R w KQ - 3 6";
    
    TEST("MoveGen::Generate: london piece movements - white king", [&fen](){
        Board b(fen);
        BitBoard expected = 0x00'00'00'00'00'00'18'70;
        
        MoveGen gen;
        BitBoard actual = gen.Generate(b.Pieces(), 4, b.Castling(b.Player()));
        
        TestSuite::assertEqual(expected, actual);
    });
    
    TEST("MoveGen::Generate: london piece movements - white queen", [&fen](){
        Board b(fen);
        BitBoard expected = 0x00'00'00'80'41'22'1c'0c;
        
        MoveGen gen;
        BitBoard actual = gen.Generate(b.Pieces(), 3, b.Castling(b.Player()));
        
        TestSuite::assertEqual(expected, actual);
    });
    
    TEST("MoveGen::Generate: london piece movements - white pinned pawn e3", [&fen](){
        Board b(fen);
        BitBoard expected = 0x00'00'00'00'00'04'00'00;
        
        MoveGen gen;
        BitBoard actual = gen.Generate(b.Pieces(), 18, b.Castling(b.Player()));
        
        TestSuite::assertEqual(expected, actual);
    });
}

static void WhitePuzzleTests()
{
    constexpr const char* fen = "2kr2nr/B5p1/2p5/1pb1p3/4P1b1/1BN3P1/PP6/R4RK w - - 1 2";
    
    TEST("MoveGen::Generate: puzzle king in check - move dark bishop", [&fen](){
        BitBoard expected = 0x00'01'00'04'00'00'00'00;
        const Board b(fen);
        
        MoveGen gen;
        BitBoard actual = gen.Generate(b.Pieces(), 48, b.Castling(b.Player()));
        
        TestSuite::assertEqual(expected, actual);
    });
    
    TEST("MoveGen::Generate: puzzle king in check - move light bishop", [&fen](){
        BitBoard expected = 0x00'00'00'00'00'02'00'00;
        const Board b(fen);
        
        MoveGen gen;
        BitBoard actual = gen.Generate(b.Pieces(), 48, b.Castling(b.Player()));

        TestSuite::assertEqual(expected, actual);
    });
    
    TEST("MoveGen::Generate: puzzle king in check - move king", [&fen](){
        BitBoard expected = 0x00'00'00'00'00'00'40'40;
        const Board b(fen);
        
        MoveGen gen;
        BitBoard actual = gen.Generate(b.Pieces(), 48, b.Castling(b.Player()));
        
        TestSuite::assertEqual(expected, actual);
    });
    
    TEST("MoveGen::Generate: puzzle king in check - move rook", [&fen](){
        BitBoard expected = 0x00'00'00'00'00'00'20'20;
        const Board b(fen);
        
        MoveGen gen;
        BitBoard actual = gen.Generate(b.Pieces(), 48, b.Castling(b.Player()));
        
        TestSuite::assertEqual(expected, actual);
    });
}

static void BlackLondonTests()
{
    constexpr const char* fen = "r1bq1rk1/ppp2ppB/2n1pn2/b2pN3/3P1B2/2P1P3/PP3PPP/RN1QK2R b KQ - 0 7";

    TEST("MoveGen::Generate: london piece movements - black king", [&fen](){
        BitBoard expected = 0xc0'80'00'00'00'00'00'00;
        const Board b(fen);

        MoveGen gen;
        BitBoard actual = gen.Generate(b.Pieces(), 62, b.Castling(b.Player()));
        
        TestSuite::assertEqual(expected, actual);
    });

    TEST("MoveGen::Generate: london piece movements - black pawn g7", [&fen](){
        BitBoard expected = 0x00'40'00'00'00'00'00'00;
        const Board b(fen);

        MoveGen gen;
        BitBoard actual = gen.Generate(b.Pieces(), 54, b.Castling(b.Player()));
        
        TestSuite::assertEqual(expected, actual);
    });

    TEST("MoveGen::Generate: london piece movements - black pinned pawn e3", [&fen](){
        BitBoard expected = 0x00'00'00'00'00'04'00'00;
        const Board b(fen);

        MoveGen gen;
        BitBoard actual = gen.Generate(b.Pieces(), 18, b.Castling(b.Player()));
        
        TestSuite::assertEqual(expected, actual);
    });
}

static void MiscTests()
{
    TEST("MoveGen::Generate: kings indian piece movements - move white on black turn", [](){
        BitBoard expected = (u64)(0x00'00'00'00'02'02'02'00);
        Board b("r1bq1rk1/ppp2ppB/2n1pn2/b2pN3/3P1B2/2P1P3/PP3PPP/RN1QK2R b KQ - 0 7");
        
        MoveGen gen;
        BitBoard actual = gen.Generate(b.Pieces(), 9, b.Castling(b.Player()));

        TestSuite::assertEqual(expected, actual);
    });
}

void MoveGenTests()
{
    WhiteLondonTests();
    WhitePuzzleTests();

    BlackLondonTests();

    MiscTests();
}

