#include "TestSuite/TestSuite.h"

#include "Board.h"
#include "Convert.h"
#include "MoveGen.h"

static void BreakMoveGen()
{
    TEST("MoveGen::Generate: null pieces", [](){
        BitBoard expected = MoveGen::INVALID;
        MoveGen gen;

        BitBoard actual = gen.Generate(nullptr, 3, 12);

        TestSuite::assertEqual(expected, actual);
    });

    TEST("MoveGen::Generate: index out of bounds", [](){
        BitBoard expected = MoveGen::INVALID;
        Piece pieces[64];
        MoveGen gen;

        BitBoard actual = gen.Generate(pieces, 64, 12);

        TestSuite::assertEqual(expected, actual);
    });

    TEST("MoveGen::Generate: too much castling", [](){
        BitBoard expected = MoveGen::INVALID;
        Board b(DEFAULT_FEN);
        MoveGen gen;
        constexpr u8 castling = (
            (u8)Enums::Castling::Black_King | (u8)Enums::Castling::Black_Queen |
            (u8)Enums::Castling::White_King | (u8)Enums::Castling::White_Queen
        );

        BitBoard actual = gen.Generate(b.Pieces().data(), 3, castling);

        TestSuite::assertEqual(expected, actual);
    });
}



static void WhiteLondonTests()
{
    constexpr const char* fen = "r1bq1rk1/ppp2ppp/2n1pn2/b2pN3/3P1B2/2PBP3/PP3PPP/RN1QK2R w KQ - 3 6";

    TEST("MoveGen::Generate: london piece movements - white king", [&fen](){
        Board b(fen);
        BitBoard expected = 0x00'00'00'00'00'00'18'70;

        MoveGen gen;
        BitBoard actual = gen.Generate(b.Pieces().data(), 4, b.Castling(b.Player()));

        TestSuite::assertEqual(expected, actual);
    });

    TEST("MoveGen::Generate: london piece movements - white queen", [&fen](){
        Board b(fen);
        BitBoard expected = 0x00'00'00'80'41'22'1c'0c;

        MoveGen gen;
        BitBoard actual = gen.Generate(b.Pieces().data(), 3, b.Castling(b.Player()));

        TestSuite::assertEqual(expected, actual);
    });

    TEST("MoveGen::Generate: london piece movements - white pinned pawn e3", [&fen](){
        Board b(fen);
        BitBoard expected = 0x00'00'00'00'00'04'00'00;

        MoveGen gen;
        BitBoard actual = gen.Generate(b.Pieces().data(), 18, b.Castling(b.Player()));

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
        BitBoard actual = gen.Generate(b.Pieces().data(), 48, b.Castling(b.Player()));

        TestSuite::assertEqual(expected, actual);
    });

    TEST("MoveGen::Generate: puzzle king in check - move light bishop", [&fen](){
        BitBoard expected = 0x00'00'00'00'00'02'00'00;
        const Board b(fen);

        MoveGen gen;
        BitBoard actual = gen.Generate(b.Pieces().data(), 17, b.Castling(b.Player()));

        TestSuite::assertEqual(expected, actual);
    });

    TEST("MoveGen::Generate: puzzle king in check - move king", [&fen](){
        BitBoard expected = 0x00'00'00'00'00'00'40'40;
        const Board b(fen);

        MoveGen gen;
        BitBoard actual = gen.Generate(b.Pieces().data(), 6, b.Castling(b.Player()));

        TestSuite::assertEqual(expected, actual);
    });

    TEST("MoveGen::Generate: puzzle king in check - move rook", [&fen](){
        BitBoard expected = 0x00'00'00'00'00'00'20'20;
        const Board b(fen);

        MoveGen gen;
        BitBoard actual = gen.Generate(b.Pieces().data(), 5, b.Castling(b.Player()));

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
        BitBoard actual = gen.Generate(b.Pieces().data(), 62, b.Castling(b.Player()));

        TestSuite::assertEqual(expected, actual);
    });

    TEST("MoveGen::Generate: london piece movements - black pawn g7", [&fen](){
        BitBoard expected = 0x00'40'00'00'00'00'00'00;
        const Board b(fen);

        MoveGen gen;
        BitBoard actual = gen.Generate(b.Pieces().data(), 54, b.Castling(b.Player()));

        TestSuite::assertEqual(expected, actual);
    });

    TEST("MoveGen::Generate: london piece movements - black pinned pawn e3", [&fen](){
        BitBoard expected = 0x00'00'00'00'00'04'00'00;
        const Board b(fen);

        MoveGen gen;
        BitBoard actual = gen.Generate(b.Pieces().data(), 18, b.Castling(b.Player()));

        TestSuite::assertEqual(expected, actual);
    });
}

static void BlackPuzzleTests()
{

}



static void CheckTests()
{
    TEST("MoveGen::Generate: knight check - bishop", [](){
        BitBoard expected = 0x00'00'00'40'00'10'00'00;
        Board b("rnbq1r2/ppp1pp1k/3p2p1/6N1/2P5/4b3/PP3PPP/R1BQKB1R b KQ - 1 9");

        MoveGen gen;
        BitBoard actual = gen.Generate(b.Pieces().data(), 20, b.Castling(b.Player()));

        TestSuite::assertEqual(expected, actual);
    });

    TEST("MoveGen::Generate: knight check - pawn", [](){
        BitBoard expected = 0x00'10'00'00'00'00'00'00;
        Board b("rnbq1r2/ppp1pp1k/3p2p1/6N1/2P5/4b3/PP3PPP/R1BQKB1R b KQ - 1 9");

        MoveGen gen;
        BitBoard actual = gen.Generate(b.Pieces().data(), 52, b.Castling(b.Player()));

        TestSuite::assertEqual(expected, actual);
    });

    TEST("MoveGen::Generate: knight check - king", [](){
        BitBoard expected = 0xc0'c0'80'00'00'00'00'00;
        Board b("rnbq1r2/ppp1pp1k/3p2p1/6N1/2P5/4b3/PP3PPP/R1BQKB1R b KQ - 1 9");

        MoveGen gen;
        BitBoard actual = gen.Generate(b.Pieces().data(), 55, b.Castling(b.Player()));

        TestSuite::assertEqual(expected, actual);
    });

    TEST("MoveGen::Generate: pawn check - king", [](){
        BitBoard expected = 0xc0'c0'c0'00'00'00'00'00;
        Board b("rnbq1r2/ppp1pp1k/3p2P1/8/2P5/4b3/PP3PPP/R1BQKB1R b KQ - 1 9");

        MoveGen gen;
        BitBoard actual = gen.Generate(b.Pieces().data(), 55, b.Castling(b.Player()));

        TestSuite::assertEqual(expected, actual);
    });

    TEST("MoveGen::Generate: pawn check - pawn", [](){
        BitBoard expected = 0x00'20'40'00'00'00'00'00;
        Board b("rnbq1r2/ppp1pp1k/3p2P1/8/2P5/4b3/PP3PPP/R1BQKB1R b KQ - 1 9");

        MoveGen gen;
        BitBoard actual = gen.Generate(b.Pieces().data(), 53, b.Castling(b.Player()));

        TestSuite::assertEqual(expected, actual);
    });

    TEST("MoveGen::Generate: pawn check defended - king", [](){
        BitBoard expected = 0xc0'c0'80'00'00'00'00'00;
        Board b("rnbq1r2/ppp1pp1k/3p2P1/8/2P2N2/4b3/PP3PPP/R1BQKB1R b KQ - 1 9");

        MoveGen gen;
        BitBoard actual = gen.Generate(b.Pieces().data(), 55, b.Castling(b.Player()));

        TestSuite::assertEqual(expected, actual);
    });
}

static void MiscTests()
{
    TEST("MoveGen::Generate: kings indian piece movements - move white on black turn", [](){
        BitBoard expected = (u64)(0x00'00'00'00'02'02'02'00);
        Board b("r1bq1rk1/ppp2ppB/2n1pn2/b2pN3/3P1B2/2P1P3/PP3PPP/RN1QK2R b KQ - 0 7");

        MoveGen gen;
        BitBoard actual = gen.Generate(b.Pieces().data(), 9, b.Castling(b.Player()));

        TestSuite::assertEqual(expected, actual);
    });

    TEST("MoveGen::Generate: invalid piece", [](){
        BitBoard expected = MoveGen::INVALID;
        Board b(DEFAULT_FEN);

        MoveGen gen;
        BitBoard actual = gen.Generate(b.Pieces().data(), 34, b.Castling(b.Player()));

        TestSuite::assertEqual(expected, actual);
    });

    TEST("MoveGen::Generate: en passant", [](){
        BitBoard expected = 0x00'00'18'08'00'00'00'00;
        Board b("rnbqkbnr/pppp1ppp/8/3Pp3/8/8/PPP1PPPP/RNBQKBNR w KQkq e6 0 2");

        MoveGen gen;
        BitBoard actual = gen.Generate(b.Pieces().data(), 35, b.Castling(b.Player()));

        TestSuite::assertEqual(expected, actual);
    });

    TEST("MoveGen::Generate: en passant", [](){
        BitBoard expected = 0x00'00'30'10'00'00'00'00;
        Board b(DEFAULT_FEN);
        std::vector<std::string> moves = {"e2e4", "d7d5", "e4e5", "f7f5"};
        for (const auto& move : moves) {
            TestSuite::assertTrue(b.MakeMove(move));
        }
        MoveGen gen;
        BitBoard actual = gen.Generate(b.Pieces().data(), 36, b.Castling(b.Player()));

        TestSuite::assertEqual(expected, actual);
    });
}

void MoveGenTests()
{
    BreakMoveGen();

    WhiteLondonTests();
    WhitePuzzleTests();

    BlackLondonTests();
    BlackPuzzleTests();

    MiscTests();
    CheckTests();
}

