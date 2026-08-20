#include "TestSuite/Assert.h"
#include "TestSuite/TestSuite.h"

#include <print>
#include <string>
#include <vector>

#include "MoveGen/MoveGen.h"
#include "Types/Position.h"
#include "Utils/Convert.h"

static void InitialMoves()
{

    TEST("MoveGen::Generate: white initial - r1", []() {
        BitBoard expected(0x00'00'00'00'00'00'00'01);

        Position pos(DEFAULT_FEN);
        MoveList list;
        MoveGen::Generate<QUIETS>(pos, list);
        MoveGen::Generate<CAPTURES>(pos, list);
        list.Legalize(pos);

        BitBoard actual = list.ToBB(Square(0));
        actual |= Square(0);

        Assert::Equal(actual, expected);
    });

    TEST("MoveGen::Generate: white initial - n1", []() {
        BitBoard expected(0x00'00'00'00'00'05'00'02);

        Position pos(DEFAULT_FEN);
        MoveList list;
        MoveGen::Generate<QUIETS>(pos, list);
        MoveGen::Generate<CAPTURES>(pos, list);
        list.Legalize(pos);

        BitBoard actual = list.ToBB(Square(1));
        actual |= Square(1);

        Assert::Equal(actual, expected);
    });

    TEST("MoveGen::Generate: white initial - b1", []() {
        BitBoard expected(0x00'00'00'00'00'00'00'04);

        Position pos(DEFAULT_FEN);
        MoveList list;
        MoveGen::Generate<QUIETS>(pos, list);
        MoveGen::Generate<CAPTURES>(pos, list);
        list.Legalize(pos);

        BitBoard actual = list.ToBB(Square(2));
        actual |= Square(2);

        Assert::Equal(actual, expected);
    });

    TEST("MoveGen::Generate: white initial - q", []() {
        BitBoard expected(0x00'00'00'00'00'00'00'08);

        Position pos(DEFAULT_FEN);
        MoveList list;
        MoveGen::Generate<QUIETS>(pos, list);
        MoveGen::Generate<CAPTURES>(pos, list);
        list.Legalize(pos);

        BitBoard actual = list.ToBB(Square(3));
        actual |= Square(3);

        Assert::Equal(actual, expected);
    });

    TEST("MoveGen::Generate: white initial - k", []() {
        BitBoard expected(0x00'00'00'00'00'00'00'10);

        Position pos(DEFAULT_FEN);
        MoveList list;
        MoveGen::Generate<QUIETS>(pos, list);
        MoveGen::Generate<CAPTURES>(pos, list);
        list.Legalize(pos);

        BitBoard actual = list.ToBB(Square(4));
        actual |= Square(4);

        Assert::Equal(actual, expected);
    });

    TEST("MoveGen::Generate: white initial - b2", []() {
        BitBoard expected(0x00'00'00'00'00'00'00'20);

        Position pos(DEFAULT_FEN);
        MoveList list;
        MoveGen::Generate<QUIETS>(pos, list);
        MoveGen::Generate<CAPTURES>(pos, list);
        list.Legalize(pos);

        BitBoard actual = list.ToBB(Square(5));
        actual |= Square(5);

        Assert::Equal(actual, expected);
    });

    TEST("MoveGen::Generate: white initial - n2", []() {
        BitBoard expected(0x00'00'00'00'00'a0'00'40);

        Position pos(DEFAULT_FEN);
        MoveList list;
        MoveGen::Generate<QUIETS>(pos, list);
        MoveGen::Generate<CAPTURES>(pos, list);
        list.Legalize(pos);

        BitBoard actual = list.ToBB(Square(6));
        actual |= Square(6);

        Assert::Equal(actual, expected);
    });

    TEST("MoveGen::Generate: white initial - r2", []() {
        BitBoard expected(0x00'00'00'00'00'00'00'80);

        Position pos(DEFAULT_FEN);
        MoveList list;
        MoveGen::Generate<QUIETS>(pos, list);
        MoveGen::Generate<CAPTURES>(pos, list);
        list.Legalize(pos);

        BitBoard actual = list.ToBB(Square(7));
        actual |= Square(7);

        Assert::Equal(actual, expected);
    });

    TEST("MoveGen::Generate: white initial - p1", []() {
        BitBoard expected(0x00'00'00'00'01'01'01'00);

        Position pos(DEFAULT_FEN);
        MoveList list;
        MoveGen::Generate<QUIETS>(pos, list);
        MoveGen::Generate<CAPTURES>(pos, list);
        list.Legalize(pos);

        BitBoard actual = list.ToBB(Square(8));
        actual |= Square(8);

        Assert::Equal(actual, expected);
    });

    TEST("MoveGen::Generate: white initial - p2", []() {
        BitBoard expected(0x00'00'00'00'02'02'02'00);

        Position pos(DEFAULT_FEN);
        MoveList list;
        MoveGen::Generate<QUIETS>(pos, list);
        MoveGen::Generate<CAPTURES>(pos, list);
        list.Legalize(pos);

        BitBoard actual = list.ToBB(Square(9));
        actual |= Square(9);

        Assert::Equal(actual, expected);
    });

    TEST("MoveGen::Generate: white initial - p3", []() {
        BitBoard expected(0x00'00'00'00'04'04'04'00);

        Position pos(DEFAULT_FEN);
        MoveList list;
        MoveGen::Generate<QUIETS>(pos, list);
        MoveGen::Generate<CAPTURES>(pos, list);
        list.Legalize(pos);

        BitBoard actual = list.ToBB(Square(10));
        actual |= Square(10);

        Assert::Equal(actual, expected);
    });

    TEST("MoveGen::Generate: white initial - p4", []() {
        BitBoard expected(0x00'00'00'00'08'08'08'00);

        Position pos(DEFAULT_FEN);
        MoveList list;
        MoveGen::Generate<QUIETS>(pos, list);
        MoveGen::Generate<CAPTURES>(pos, list);
        list.Legalize(pos);

        BitBoard actual = list.ToBB(Square(11));
        actual |= Square(11);

        Assert::Equal(actual, expected);
    });

    TEST("MoveGen::Generate: white initial - p5", []() {
        BitBoard expected(0x00'00'00'00'10'10'10'00);

        Position pos(DEFAULT_FEN);
        MoveList list;
        MoveGen::Generate<QUIETS>(pos, list);
        MoveGen::Generate<CAPTURES>(pos, list);
        list.Legalize(pos);

        BitBoard actual = list.ToBB(Square(12));
        actual |= Square(12);

        Assert::Equal(actual, expected);
    });

    TEST("MoveGen::Generate: white initial - p6", []() {
        BitBoard expected(0x00'00'00'00'20'20'20'00);

        Position pos(DEFAULT_FEN);
        MoveList list;
        MoveGen::Generate<QUIETS>(pos, list);
        MoveGen::Generate<CAPTURES>(pos, list);
        list.Legalize(pos);

        BitBoard actual = list.ToBB(Square(13));
        actual |= Square(13);

        Assert::Equal(actual, expected);
    });

    TEST("MoveGen::Generate: white initial - p7", []() {
        BitBoard expected(0x00'00'00'00'40'40'40'00);

        Position pos(DEFAULT_FEN);
        MoveList list;
        MoveGen::Generate<QUIETS>(pos, list);
        MoveGen::Generate<CAPTURES>(pos, list);
        list.Legalize(pos);

        BitBoard actual = list.ToBB(Square(14));
        actual |= Square(14);

        Assert::Equal(actual, expected);
    });

    TEST("MoveGen::Generate: white initial - p8", []() {
        BitBoard expected(0x00'00'00'00'80'80'80'00);

        Position pos(DEFAULT_FEN);
        MoveList list;
        MoveGen::Generate<QUIETS>(pos, list);
        MoveGen::Generate<CAPTURES>(pos, list);
        list.Legalize(pos);

        BitBoard actual = list.ToBB(Square(15));
        actual |= Square(15);

        Assert::Equal(actual, expected);
    });
}

//

static void WhiteLondonTests()
{
    static constexpr const char* fen =
        "r1bq1rk1/ppp2ppp/2n1pn2/b2pN3/3P1B2/2PBP3/PP3PPP/RN1QK2R w KQ - 3 6";

    TEST("MoveGen::Generate: london piece movements - white king", []() {
        BitBoard expected = 0x00'00'00'00'00'00'18'70;

        Position pos(fen);
        MoveList list;
        MoveGen::Generate<QUIETS>(pos, list);
        MoveGen::Generate<CAPTURES>(pos, list);
        list.Legalize(pos);

        BitBoard actual = list.ToBB(Square(4));
        actual |= Square(4);

        Assert::Equal(actual, expected);
    });

    TEST("MoveGen::Generate: london piece movements - white queen", []() {
        BitBoard expected = 0x00'00'00'80'41'22'1c'0c;

        Position pos(fen);
        MoveList list;
        MoveGen::Generate<QUIETS>(pos, list);
        MoveGen::Generate<CAPTURES>(pos, list);
        list.Legalize(pos);

        BitBoard actual = list.ToBB(Square(3));
        actual |= Square(3);

        Assert::Equal(actual, expected);
    });

    TEST("MoveGen::Generate: london piece movements - white pinned pawn e3", []() {
        BitBoard expected = 0x00'00'00'00'00'04'00'00;

        Position pos(fen);
        MoveList list;
        MoveGen::Generate<QUIETS>(pos, list);
        MoveGen::Generate<CAPTURES>(pos, list);
        list.Legalize(pos);

        BitBoard actual = list.ToBB(Square(18));
        actual |= Square(18);

        Assert::Equal(actual, expected);
    });
}

static void WhitePuzzleTests()
{
    static constexpr const char* fen = "2kr2nr/B5p1/2p5/1pb1p3/4P1b1/1BN3P1/PP6/R4RK1 w - - 1 2";

    TEST("MoveGen::Generate: puzzle king in check - move dark bishop", []() {
        BitBoard expected = 0x00'01'00'04'00'00'00'00;

        Position pos(fen);
        MoveList list;
        MoveGen::Generate<QUIETS>(pos, list);
        MoveGen::Generate<CAPTURES>(pos, list);
        list.Legalize(pos);

        BitBoard actual = list.ToBB(Square(48));
        actual |= Square(48);

        Assert::Equal(actual, expected);
    });

    TEST("MoveGen::Generate: puzzle king in check - move light bishop", []() {
        BitBoard expected = 0x00'00'00'00'00'02'00'00;

        Position pos(fen);
        MoveList list;
        MoveGen::Generate<QUIETS>(pos, list);
        MoveGen::Generate<CAPTURES>(pos, list);
        list.Legalize(pos);

        BitBoard actual = list.ToBB(Square(17));
        actual |= Square(17);

        Assert::Equal(actual, expected);
    });

    TEST("MoveGen::Generate: puzzle king in check - move king", []() {
        BitBoard expected = 0x00'00'00'00'00'00'40'40;

        Position pos(fen);
        MoveList list;
        MoveGen::Generate<QUIETS>(pos, list);
        MoveGen::Generate<CAPTURES>(pos, list);
        list.Legalize(pos);

        BitBoard actual = list.ToBB(Square(6));
        actual |= Square(6);

        Assert::Equal(actual, expected);
    });

    TEST("MoveGen::Generate: puzzle king in check - move rook", []() {
        BitBoard expected = 0x00'00'00'00'00'00'20'20;

        Position pos(fen);
        MoveList list;
        MoveGen::Generate<QUIETS>(pos, list);
        MoveGen::Generate<CAPTURES>(pos, list);
        list.Legalize(pos);

        BitBoard actual = list.ToBB(Square(5));
        actual |= Square(5);

        Assert::Equal(actual, expected);
    });
}

//

// static void BlackLondonTests()
// {
//     constexpr const char* fen =
//         "r1bq1rk1/ppp2ppB/2n1pn2/b2pN3/3P1B2/2P1P3/PP3PPP/RN1QK2R b KQ - 0 7";

//     TEST("MoveGen::Generate: london piece movements - black king", [&fen]() {
//         BitBoard    expected = 0xc0'80'00'00'00'00'00'00;
//         const Board b(fen);

//         MoveGen gen;
//         gen.Generate(b, BLACK);
//         BitBoard actual = gen.GetMoves(62);

//         Assert::Equal(actual, expected);
//     });

//     TEST("MoveGen::Generate: london piece movements - black pawn g7", [&fen]() {
//         BitBoard    expected = 0x00'40'00'00'00'00'00'00;
//         const Board b(fen);

//         MoveGen gen;
//         gen.Generate(b, BLACK);
//         BitBoard actual = gen.GetMoves(54);

//         Assert::Equal(actual, expected);
//     });

//     TEST("MoveGen::Generate: london piece movements - black pinned pawn e3", [&fen]() {
//         BitBoard    expected = 0x00'40'00'00'00'00'00'00;
//         const Board b(fen);

//         MoveGen gen;
//         gen.Generate(b, BLACK);
//         BitBoard actual = gen.GetMoves(54);

//         Assert::Equal(actual, expected);
//     });
// }

// static void BlackPuzzleTests() {}

// //

// static void CheckTests()
// {
//     TEST("MoveGen::Generate: knight check - bishop", []() {
//         BitBoard expected = 0x00'00'00'40'00'10'00'00;
//         Board    b("rnbq1r2/ppp1pp1k/3p2p1/6N1/2P5/4b3/PP3PPP/R1BQKB1R b KQ - 1 9");

//         MoveGen gen;
//         gen.Generate(b, BLACK);
//         BitBoard actual = gen.GetMoves(20);

//         Assert::Equal(actual, expected);
//     });

//     TEST("MoveGen::Generate: knight check - pawn", []() {
//         BitBoard expected = 0x00'10'00'00'00'00'00'00;
//         Board    b("rnbq1r2/ppp1pp1k/3p2p1/6N1/2P5/4b3/PP3PPP/R1BQKB1R b KQ - 1 9");

//         MoveGen gen;
//         gen.Generate(b, BLACK);
//         BitBoard actual = gen.GetMoves(52);

//         Assert::Equal(actual, expected);
//     });

//     TEST("MoveGen::Generate: knight check - king", []() {
//         BitBoard expected = 0xc0'c0'80'00'00'00'00'00;
//         Board    b("rnbq1r2/ppp1pp1k/3p2p1/6N1/2P5/4b3/PP3PPP/R1BQKB1R b KQ - 1 9");

//         MoveGen gen;
//         gen.Generate(b, BLACK);
//         BitBoard actual = gen.GetMoves(55);

//         Assert::Equal(actual, expected);
//     });

//     TEST("MoveGen::Generate: pawn check - king", []() {
//         BitBoard expected = 0xc0'c0'c0'00'00'00'00'00;
//         Board    b("rnbq1r2/ppp1pp1k/3p2P1/8/2P5/4b3/PP3PPP/R1BQKB1R b KQ - 1 9");

//         MoveGen gen;
//         gen.Generate(b, BLACK);
//         BitBoard actual = gen.GetMoves(55);

//         Assert::Equal(actual, expected);
//     });

//     TEST("MoveGen::Generate: pawn check - pawn", []() {
//         BitBoard expected = 0x00'20'40'00'00'00'00'00;
//         Board    b("rnbq1r2/ppp1pp1k/3p2P1/8/2P5/4b3/PP3PPP/R1BQKB1R b KQ - 1 9");

//         MoveGen gen;
//         gen.Generate(b, BLACK);
//         BitBoard actual = gen.GetMoves(53);

//         Assert::Equal(actual, expected);
//     });

//     TEST("MoveGen::Generate: pawn check defended - king", []() {
//         BitBoard expected = 0xc0'c0'80'00'00'00'00'00;
//         Board    b("rnbq1r2/ppp1pp1k/3p2P1/8/2P2N2/4b3/PP3PPP/R1BQKB1R b KQ - 1 9");

//         MoveGen gen;
//         gen.Generate(b, BLACK);
//         BitBoard actual = gen.GetMoves(55);

//         Assert::Equal(actual, expected);
//     });

//     TEST("MoveGen::Generate: bishop check - knight", []() {
//         BitBoard expected = 0x00'00'04'02'08'00'00'00;
//         Board    b("r1bqkbnr/ppp2ppp/3p4/4p3/B2nP3/5N2/PPPPQPPP/RNB1K2R b KQkq - 3 5");

//         MoveGen gen;
//         gen.Generate(b, BLACK);
//         BitBoard actual = gen.GetMoves(27);

//         Assert::Equal(actual, expected);
//     });

//     TEST("MoveGen::Generate: rook check - king", []() {
//         BitBoard expected = 0x00'00'00'00'00'00'28'38;
//         Board    b("4r3/8/8/8/8/8/8/4K2R w K - 0 1");

//         MoveGen gen;
//         gen.Generate(b, WHITE);
//         BitBoard actual = gen.GetMoves(4);

//         Assert::Equal(actual, expected);
//     });

//     TEST("MoveGen::Generate: rook check - rook", []() {
//         BitBoard expected = 0x00'00'00'00'00'00'11'00;
//         Board    b("4r3/8/8/8/8/8/R7/4K2R w K - 0 1");

//         MoveGen gen;
//         gen.Generate(b, WHITE);
//         BitBoard actual = gen.GetMoves(8);

//         Assert::Equal(actual, expected);
//     });

//     TEST("MoveGen::Generate: queen check - bishop", []() {
//         BitBoard expected = 0x20'40'00'00'00'00'00'00;
//         Board    b("rnbqk1Q1/ppppp1bp/8/8/8/8/PPPP1PPP/RNBQKBNR b KQq - 0 6");

//         MoveGen gen;
//         gen.Generate(b, BLACK);
//         BitBoard actual = gen.GetMoves(54);

//         Assert::Equal(actual, expected);
//     });

//     TEST("MoveGen::Generate: queen checkmate - pawn", []() {
//         BitBoard expected = 0x00'01'00'00'00'00'00'00;
//         Board    b("rnbqkbnr/ppppp2p/5p2/6pQ/2B1P3/8/PPPP1PPP/RNB1K1NR b KQkq - 1 3");

//         MoveGen gen;
//         gen.Generate(b, BLACK);
//         BitBoard actual = gen.GetMoves(48);

//         Assert::Equal(actual, expected);
//     });

//     TEST("MoveGen::Generate: rook check - skewer", []() {
//         BitBoard expected = 0x00'14'0c'04'00'00'00'00;
//         Board    b("8/1P6/3k4/8/1b6/p2RnK2/B6p/B7 b - - 9 83");

//         MoveGen gen;
//         gen.Generate(b, BLACK);
//         BitBoard actual = gen.GetMoves(43);

//         Assert::Equal(actual, expected);
//     });
// }

// static void DoubleCheckTests()
// {
//     TEST("MoveGen::Generate: double check - king", []() {
//         BitBoard expected = 0xc0'c0'c0'00'00'00'00'00;
//         Board    b("rnbq1r2/ppp1pp1k/3p2P1/6N1/2P5/4b3/PP3PPP/R1BQKB1R b KQ - 0 9");

//         MoveGen gen;
//         gen.Generate(b, BLACK);
//         BitBoard actual = gen.GetMoves(55);

//         Assert::Equal(actual, expected);
//     });

//     TEST("MoveGen::Generate: double check defended pawn - king", []() {
//         BitBoard expected = 0xc0'c0'80'00'00'00'00'00;
//         Board    b("rnbq1r2/ppp1pp1k/3p2P1/6N1/2P4N/4b3/PP3PPP/R1BQKB1R b KQ - 0 9");

//         MoveGen gen;
//         gen.Generate(b, BLACK);
//         BitBoard actual = gen.GetMoves(55);

//         Assert::Equal(actual, expected);
//     });

//     TEST("MoveGen::Generate: double check - bishop", []() {
//         BitBoard expected = 0x00'00'00'00'00'10'00'00;
//         Board    b("rnbq1r2/ppp1pp1k/3p2P1/6N1/2P4N/4b3/PP3PPP/R1BQKB1R b KQ - 0 9");

//         MoveGen gen;
//         gen.Generate(b, BLACK);
//         BitBoard actual = gen.GetMoves(20);

//         Assert::Equal(actual, expected);
//     });

//     TEST("MoveGen::Generate: double check - pawn", []() {
//         BitBoard expected = 0x00'20'00'00'00'00'00'00;
//         Board    b("rnbq1r2/ppp1pp1k/3p2P1/6N1/2P4N/4b3/PP3PPP/R1BQKB1R b KQ - 0 9");

//         MoveGen gen;
//         gen.Generate(b, BLACK);
//         BitBoard actual = gen.GetMoves(53);

//         Assert::Equal(actual, expected);
//     });
// }

// static void MiscTests()
// {
//     TEST("MoveGen::Generate: kings indian piece movements - move white on black turn", []() {
//         BitBoard expected = (u64)(0x00'00'00'00'02'02'02'00);
//         Board    b("r1bq1rk1/ppp2ppB/2n1pn2/b2pN3/3P1B2/2P1P3/PP3PPP/RN1QK2R b KQ - 0 7");

//         MoveGen gen;
//         gen.Generate(b, WHITE);
//         BitBoard actual = gen.GetMoves(9);

//         Assert::Equal(actual, expected);
//     });

//     TEST("MoveGen::Generate: invalid piece", []() {
//         BitBoard expected = MoveGen::INVALID;
//         Board    b(DEFAULT_FEN);

//         MoveGen gen;
//         gen.Generate(b, WHITE);
//         BitBoard actual = gen.GetMoves(34);

//         Assert::Equal(actual, expected);
//     });

//     TEST("MoveGen::Generate: en passant - from fen", []() {
//         BitBoard expected = 0x00'00'18'08'00'00'00'00;
//         Board    b("rnbqkbnr/pppp1ppp/8/3Pp3/8/8/PPP1PPPP/RNBQKBNR w KQkq e6 0 2");

//         MoveGen gen;
//         gen.Generate(b, WHITE);
//         BitBoard actual = gen.GetMoves(35);

//         Assert::Equal(actual, expected);
//     });

//     TEST("MoveGen::Generate: en passant - make moves", []() {
//         BitBoard                 expected = 0x00'00'30'10'00'00'00'00;
//         Board                    b(DEFAULT_FEN);
//         std::vector<std::string> moves = {"e2e4", "d7d5", "e4e5", "f7f5"};
//         for (const auto& move : moves) {
//             Assert::True(b.MakeMove(move));
//         }
//         MoveGen gen;
//         gen.Generate(b, WHITE);
//         BitBoard actual = gen.GetMoves(36);

//         Assert::Equal(actual, expected);
//     });

//     TEST("MoveGen::Generate: en passant - ensure pawn gone", []() {
//         BitBoard                 expected = 0x00'00'00'00'00'00'00'00;
//         Board                    b(DEFAULT_FEN);
//         std::vector<std::string> moves = {"e2e4", "d7d5", "e4e5", "f7f5", "e5f6"};
//         for (const auto& move : moves) {
//             Assert::True(b.MakeMove(move));
//         }
//         MoveGen gen;
//         gen.Generate(b, BLACK);
//         BitBoard actual = gen.GetMoves(37);

//         Assert::Equal(actual, expected);
//     });

//     TEST("MoveGen::Generate: en passant - only 1 way en passant", []() {
//         BitBoard expected = 0x00'00'60'20'00'00'00'00;
//         Board    b("rnbqkbnr/ppppp2p/8/5Pp1/8/8/PPPP1PPP/RNBQKBNR w KQkq g6 0 2");

//         MoveGen gen;
//         gen.Generate(b, WHITE);
//         BitBoard actual = gen.GetMoves(37);

//         Assert::Equal(actual, expected);
//     });

//     TEST("MoveGen::Generate: en passant - enemy pinned", []() {
//         BitBoard expected = 0x00'00'10'10'00'00'00'00;
//         Board    b("k7/8/6b1/4Pp2/8/8/8/1K6 w - f6 0 1");

//         MoveGen gen;
//         gen.Generate(b, WHITE);
//         BitBoard actual = gen.GetMoves(36);

//         Assert::Equal(actual, expected);
//     });

//     TEST("MoveGen::Generate: castle through check", []() {
//         BitBoard expected = 0x00'00'00'00'00'00'18'18;
//         Board    b("5r2/8/8/8/8/8/8/4K2R w K - 0 1");

//         MoveGen gen;
//         gen.Generate(b, WHITE);
//         BitBoard actual = gen.GetMoves(4);

//         Assert::Equal(actual, expected);
//     });

//     TEST("MoveGen::Generate: random rook", []() {
//         BitBoard expected = 0x00'00'80'80'80'f0'80'00;
//         Board    b("2k2r2/ppp3pp/3b4/8/1P6/2P1R2r/P2P1P1P/R1B3K1 b - - 2 18");
//         MoveGen  gen;

//         gen.Generate(b, BLACK);
//         BitBoard actual = gen.GetMoves(23);

//         Assert::Equal(actual, expected);
//     });

//     TEST("MoveGen::Generate: Random queen", []() {
//         BitBoard expected = 0x00'00'00'00'00'10'38'00;
//         Board    b("8/4Np1k/p4n2/1p2p1p1/2P1P1Pp/P3QP2/1Pq3KP/8 w - - 6 42");
//         MoveGen  gen;

//         gen.Generate(b, WHITE);
//         BitBoard actual = gen.GetMoves(20);

//         Assert::Equal(actual, expected);
//     });
// }

// //

void MoveGenTests()
{
    InitialMoves();

    WhiteLondonTests();
    WhitePuzzleTests();

    //     BlackLondonTests();
    //     BlackPuzzleTests();

    //     CheckTests();
    //     DoubleCheckTests();
    //     MiscTests();
}
