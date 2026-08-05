#include "TestSuite/Assert.h"
#include "TestSuite/TestSuite.h"

#include <chrono>
#include <memory>
#include <mutex>
#include <print>
#include <string>
#include <vector>

#include "Board.h"
#include "Convert.h"
#include "MoveGen/MoveGen.h"

namespace chrono = std::chrono;

static void BreakMoveGen()
{
    TEST("MoveGen::Generate: index out of bounds", []() {
        BitBoard expected = MoveGen::INVALID;
        Board    b;
        MoveGen  gen;

        gen.Generate(b, Enums::Colour::White);
        BitBoard actual = gen.GetMoves(64);

        Assert::Equal(expected, actual);
    });
}

static void InitialMoves()
{
    // White initial
    {
        Board                    board;
        std::shared_ptr<MoveGen> gen = std::make_shared<MoveGen>();
        gen.get()->Generate(board, Enums::Colour::White);

        TEST("MoveGen::Generate: white initial - r1", [gen]() {
            BitBoard expected = 0x00'00'00'00'00'00'00'01;

            BitBoard actual = gen.get()->GetMoves(2);

            Assert::Equal(actual, expected);
        });

        TEST("MoveGen::Generate: white initial - n1", [gen]() {
            BitBoard expected = 0x00'00'00'00'00'00'00'02;

            BitBoard actual = gen.get()->GetMoves(1);

            Assert::Equal(actual, expected);
        });

        TEST("MoveGen::Generate: white initial - b1", [gen]() {
            BitBoard expected = 0x00'00'00'00'00'00'00'04;

            BitBoard actual = gen.get()->GetMoves(2);

            Assert::Equal(actual, expected);
        });

        TEST("MoveGen::Generate: white initial - q", [gen]() {
            BitBoard expected = 0x00'00'00'00'00'00'00'08;

            BitBoard actual = gen.get()->GetMoves(3);

            Assert::Equal(actual, expected);
        });

        TEST("MoveGen::Generate: white initial - k", [gen]() {
            BitBoard expected = 0x00'00'00'00'00'00'00'10;

            BitBoard actual = gen.get()->GetMoves(4);

            Assert::Equal(actual, expected);
        });

        TEST("MoveGen::Generate: white initial - b2", [gen]() {
            BitBoard expected = 0x00'00'00'00'00'00'00'20;

            BitBoard actual = gen.get()->GetMoves(5);

            Assert::Equal(actual, expected);
        });

        TEST("MoveGen::Generate: white initial - n2", [gen]() {
            BitBoard expected = 0x00'00'00'00'00'00'00'40;

            BitBoard actual = gen.get()->GetMoves(6);

            Assert::Equal(actual, expected);
        });

        TEST("MoveGen::Generate: white initial - r2", [gen]() {
            BitBoard expected = 0x00'00'00'00'00'00'00'80;

            BitBoard actual = gen.get()->GetMoves(7);

            Assert::Equal(actual, expected);
        });

        TEST("MoveGen::Generate: white initial - p1", [gen]() {
            BitBoard expected = 0x00'00'00'00'01'01'01'00;

            BitBoard actual = gen.get()->GetMoves(8);

            Assert::Equal(actual, expected);
        });

        TEST("MoveGen::Generate: white initial - p2", [gen]() {
            BitBoard expected = 0x00'00'00'00'02'02'02'00;

            BitBoard actual = gen.get()->GetMoves(9);

            Assert::Equal(actual, expected);
        });

        TEST("MoveGen::Generate: white initial - p3", [gen]() {
            BitBoard expected = 0x00'00'00'00'04'04'04'00;

            BitBoard actual = gen.get()->GetMoves(10);

            Assert::Equal(actual, expected);
        });

        TEST("MoveGen::Generate: white initial - p4", [gen]() {
            BitBoard expected = 0x00'00'00'00'08'08'08'00;

            BitBoard actual = gen.get()->GetMoves(11);

            Assert::Equal(actual, expected);
        });

        TEST("MoveGen::Generate: white initial - p5", [gen]() {
            BitBoard expected = 0x00'00'00'00'10'10'10'00;

            BitBoard actual = gen.get()->GetMoves(12);

            Assert::Equal(actual, expected);
        });

        TEST("MoveGen::Generate: white initial - p6", [gen]() {
            BitBoard expected = 0x00'00'00'00'20'20'20'00;

            BitBoard actual = gen.get()->GetMoves(13);

            Assert::Equal(actual, expected);
        });

        TEST("MoveGen::Generate: white initial - p7", [gen]() {
            BitBoard expected = 0x00'00'00'00'40'40'40'00;

            BitBoard actual = gen.get()->GetMoves(14);

            Assert::Equal(actual, expected);
        });

        TEST("MoveGen::Generate: white initial - p8", [gen]() {
            BitBoard expected = 0x00'00'00'00'80'80'80'00;

            BitBoard actual = gen.get()->GetMoves(15);

            Assert::Equal(actual, expected);
        });
    }
}

//

static void WhiteLondonTests()
{
    constexpr const char* fen =
        "r1bq1rk1/ppp2ppp/2n1pn2/b2pN3/3P1B2/2PBP3/PP3PPP/RN1QK2R w KQ - 3 6";

    TEST("MoveGen::Generate: london piece movements - white king", [&fen]() {
        Board    b(fen);
        BitBoard expected = 0x00'00'00'00'00'00'18'70;

        MoveGen gen;
        gen.Generate(b, Enums::Colour::White);
        BitBoard actual = gen.GetMoves(4);

        Assert::Equal(expected, actual);
    });

    TEST("MoveGen::Generate: london piece movements - white queen", [&fen]() {
        Board    b(fen);
        BitBoard expected = 0x00'00'00'80'41'22'1c'0c;

        MoveGen gen;
        gen.Generate(b, Enums::Colour::White);
        BitBoard actual = gen.GetMoves(3);

        Assert::Equal(expected, actual);
    });

    TEST("MoveGen::Generate: london piece movements - white pinned pawn e3", [&fen]() {
        Board    b(fen);
        BitBoard expected = 0x00'00'00'00'00'04'00'00;

        MoveGen gen;
        gen.Generate(b, Enums::Colour::White);
        BitBoard actual = gen.GetMoves(18);

        Assert::Equal(expected, actual);
    });
}

static void WhitePuzzleTests()
{
    constexpr const char* fen = "2kr2nr/B5p1/2p5/1pb1p3/4P1b1/1BN3P1/PP6/R4RK w - - 1 2";

    TEST("MoveGen::Generate: puzzle king in check - move dark bishop", [&fen]() {
        BitBoard    expected = 0x00'01'00'04'00'00'00'00;
        const Board b(fen);

        MoveGen gen;
        gen.Generate(b, Enums::Colour::White);
        BitBoard actual = gen.GetMoves(48);

        Assert::Equal(expected, actual);
    });

    TEST("MoveGen::Generate: puzzle king in check - move light bishop", [&fen]() {
        BitBoard    expected = 0x00'00'00'00'00'02'00'00;
        const Board b(fen);

        MoveGen gen;
        gen.Generate(b, Enums::Colour::White);
        BitBoard actual = gen.GetMoves(17);

        Assert::Equal(expected, actual);
    });

    TEST("MoveGen::Generate: puzzle king in check - move king", [&fen]() {
        BitBoard    expected = 0x00'00'00'00'00'00'40'40;
        const Board b(fen);

        MoveGen gen;
        gen.Generate(b, Enums::Colour::White);
        BitBoard actual = gen.GetMoves(6);

        Assert::Equal(expected, actual);
    });

    TEST("MoveGen::Generate: puzzle king in check - move rook", [&fen]() {
        BitBoard    expected = 0x00'00'00'00'00'00'20'20;
        const Board b(fen);

        MoveGen gen;
        gen.Generate(b, Enums::Colour::White);
        BitBoard actual = gen.GetMoves(5);

        Assert::Equal(expected, actual);
    });
}

//

static void BlackLondonTests()
{
    constexpr const char* fen =
        "r1bq1rk1/ppp2ppB/2n1pn2/b2pN3/3P1B2/2P1P3/PP3PPP/RN1QK2R b KQ - 0 7";

    TEST("MoveGen::Generate: london piece movements - black king", [&fen]() {
        BitBoard    expected = 0xc0'80'00'00'00'00'00'00;
        const Board b(fen);

        MoveGen gen;
        gen.Generate(b, Enums::Colour::Black);
        BitBoard actual = gen.GetMoves(62);

        Assert::Equal(expected, actual);
    });

    TEST("MoveGen::Generate: london piece movements - black pawn g7", [&fen]() {
        BitBoard    expected = 0x00'40'00'00'00'00'00'00;
        const Board b(fen);

        MoveGen gen;
        gen.Generate(b, Enums::Colour::Black);
        BitBoard actual = gen.GetMoves(54);

        Assert::Equal(expected, actual);
    });

    TEST("MoveGen::Generate: london piece movements - black pinned pawn e3", [&fen]() {
        BitBoard    expected = 0x00'00'00'00'00'04'00'00;
        const Board b(fen);

        MoveGen gen;
        gen.Generate(b, Enums::Colour::Black);
        BitBoard actual = gen.GetMoves(18);

        Assert::Equal(expected, actual);
    });
}

static void BlackPuzzleTests() {}

//

static void CheckTests()
{
    TEST("MoveGen::Generate: knight check - bishop", []() {
        BitBoard expected = 0x00'00'00'40'00'10'00'00;
        Board    b("rnbq1r2/ppp1pp1k/3p2p1/6N1/2P5/4b3/PP3PPP/R1BQKB1R b KQ - 1 9");

        MoveGen gen;
        gen.Generate(b, Enums::Colour::Black);
        BitBoard actual = gen.GetMoves(20);

        Assert::Equal(expected, actual);
    });

    TEST("MoveGen::Generate: knight check - pawn", []() {
        BitBoard expected = 0x00'10'00'00'00'00'00'00;
        Board    b("rnbq1r2/ppp1pp1k/3p2p1/6N1/2P5/4b3/PP3PPP/R1BQKB1R b KQ - 1 9");

        MoveGen gen;
        gen.Generate(b, Enums::Colour::Black);
        BitBoard actual = gen.GetMoves(52);

        Assert::Equal(expected, actual);
    });

    TEST("MoveGen::Generate: knight check - king", []() {
        BitBoard expected = 0xc0'c0'80'00'00'00'00'00;
        Board    b("rnbq1r2/ppp1pp1k/3p2p1/6N1/2P5/4b3/PP3PPP/R1BQKB1R b KQ - 1 9");

        MoveGen gen;
        gen.Generate(b, Enums::Colour::Black);
        BitBoard actual = gen.GetMoves(55);

        Assert::Equal(expected, actual);
    });

    TEST("MoveGen::Generate: pawn check - king", []() {
        BitBoard expected = 0xc0'c0'c0'00'00'00'00'00;
        Board    b("rnbq1r2/ppp1pp1k/3p2P1/8/2P5/4b3/PP3PPP/R1BQKB1R b KQ - 1 9");

        MoveGen gen;
        gen.Generate(b, Enums::Colour::Black);
        BitBoard actual = gen.GetMoves(55);

        Assert::Equal(expected, actual);
    });

    TEST("MoveGen::Generate: pawn check - pawn", []() {
        BitBoard expected = 0x00'20'40'00'00'00'00'00;
        Board    b("rnbq1r2/ppp1pp1k/3p2P1/8/2P5/4b3/PP3PPP/R1BQKB1R b KQ - 1 9");

        MoveGen gen;
        gen.Generate(b, Enums::Colour::Black);
        BitBoard actual = gen.GetMoves(53);

        Assert::Equal(expected, actual);
    });

    TEST("MoveGen::Generate: pawn check defended - king", []() {
        BitBoard expected = 0xc0'c0'80'00'00'00'00'00;
        Board    b("rnbq1r2/ppp1pp1k/3p2P1/8/2P2N2/4b3/PP3PPP/R1BQKB1R b KQ - 1 9");

        MoveGen gen;
        gen.Generate(b, Enums::Colour::Black);
        BitBoard actual = gen.GetMoves(55);

        Assert::Equal(expected, actual);
    });
}

static void DoubleCheckTests()
{
    TEST("MoveGen::Generate: double check - king", []() {
        BitBoard expected = 0xc0'c0'c0'00'00'00'00'00;
        Board    b("rnbq1r2/ppp1pp1k/3p2P1/6N1/2P5/4b3/PP3PPP/R1BQKB1R b KQ - 0 9");

        MoveGen gen;
        gen.Generate(b, Enums::Colour::Black);
        BitBoard actual = gen.GetMoves(55);

        Assert::Equal(expected, actual);
    });

    TEST("MoveGen::Generate: double check defended pawn - king", []() {
        BitBoard expected = 0xc0'c0'80'00'00'00'00'00;
        Board    b("rnbq1r2/ppp1pp1k/3p2P1/6N1/2P4N/4b3/PP3PPP/R1BQKB1R b KQ - 0 9");

        MoveGen gen;
        gen.Generate(b, Enums::Colour::Black);
        BitBoard actual = gen.GetMoves(55);

        Assert::Equal(expected, actual);
    });

    TEST("MoveGen::Generate: double check - bishop", []() {
        BitBoard expected = 0x00'00'00'00'00'10'00'00;
        Board    b("rnbq1r2/ppp1pp1k/3p2P1/6N1/2P4N/4b3/PP3PPP/R1BQKB1R b KQ - 0 9");

        MoveGen gen;
        gen.Generate(b, Enums::Colour::Black);
        BitBoard actual = gen.GetMoves(20);

        Assert::Equal(expected, actual);
    });

    TEST("MoveGen::Generate: double check - pawn", []() {
        BitBoard expected = 0x00'20'00'00'00'00'00'00;
        Board    b("rnbq1r2/ppp1pp1k/3p2P1/6N1/2P4N/4b3/PP3PPP/R1BQKB1R b KQ - 0 9");

        MoveGen gen;
        gen.Generate(b, Enums::Colour::Black);
        BitBoard actual = gen.GetMoves(53);

        Assert::Equal(expected, actual);
    });
}

static void MiscTests()
{
    TEST("MoveGen::Generate: kings indian piece movements - move white on black turn", []() {
        BitBoard expected = (u64)(0x00'00'00'00'02'02'02'00);
        Board    b("r1bq1rk1/ppp2ppB/2n1pn2/b2pN3/3P1B2/2P1P3/PP3PPP/RN1QK2R b KQ - 0 7");

        MoveGen gen;
        gen.Generate(b, Enums::Colour::White);
        BitBoard actual = gen.GetMoves(9);

        Assert::Equal(expected, actual);
    });

    TEST("MoveGen::Generate: invalid piece", []() {
        BitBoard expected = MoveGen::INVALID;
        Board    b(DEFAULT_FEN);

        MoveGen gen;
        gen.Generate(b, Enums::Colour::White);
        BitBoard actual = gen.GetMoves(34);

        Assert::Equal(expected, actual);
    });

    TEST("MoveGen::Generate: en passant - from fen", []() {
        BitBoard expected = 0x00'00'18'08'00'00'00'00;
        Board    b("rnbqkbnr/pppp1ppp/8/3Pp3/8/8/PPP1PPPP/RNBQKBNR w KQkq e6 0 2");

        MoveGen gen;
        gen.Generate(b, Enums::Colour::White);
        BitBoard actual = gen.GetMoves(35);

        Assert::Equal(expected, actual);
    });

    TEST("MoveGen::Generate: en passant - make moves", []() {
        BitBoard                 expected = 0x00'00'30'10'00'00'00'00;
        Board                    b(DEFAULT_FEN);
        std::vector<std::string> moves = {"e2e4", "d7d5", "e4e5", "f7f5"};
        for (const auto& move : moves) {
            TestSuite::assertTrue(b.MakeMove(move));
        }
        MoveGen gen;
        gen.Generate(b, Enums::Colour::Black);
        BitBoard actual = gen.GetMoves(36);

        Assert::Equal(expected, actual);
    });

    TEST("MoveGen::Generate: en passant - ensure pawn gone", []() {
        BitBoard                 expected = 0x00'00'00'00'00'00'00'00;
        Board                    b(DEFAULT_FEN);
        std::vector<std::string> moves = {"e2e4", "d7d5", "e4e5", "f7f5", "e5f6"};
        for (const auto& move : moves) {
            TestSuite::assertTrue(b.MakeMove(move));
        }
        MoveGen gen;
        gen.Generate(b, Enums::Colour::Black);
        BitBoard actual = gen.GetMoves(37);

        Assert::Equal(expected, actual);
    });
}

//

static chrono::nanoseconds RunTimeTest(std::string_view fen, Enums::Colour colour, u64 count)
{
    chrono::nanoseconds totalTime = {};
    for (u64 i = 0; i < count; i++) {
        Board   b(fen);
        MoveGen gen;

        auto start = chrono::steady_clock::now();
        gen.Generate(b, colour);
        auto end = chrono::steady_clock::now();

        auto delta = chrono::nanoseconds(end - start);
        totalTime += delta;
    }
    return totalTime;
}

static void TimeTests1()
{
    constexpr std::string_view fen   = "8/1Q3p1k/4p1q1/7p/8/1B3p1P/P4PP1/6K1 b - - 0 34";
    constexpr static u64       count = 100;
    static std::mutex          mtx;
    std::shared_ptr<FILE>      file(fopen("MoveGenV2.log", "a"), fclose);

    TEST("MoveGen::Time: 1 - white", [file]() {
        chrono::nanoseconds expected(3360);
        auto actual = chrono::nanoseconds(RunTimeTest(fen, Enums::Colour::White, count) / count);

        mtx.lock();
        std::println(file.get(), "1w:{}", actual);
        mtx.unlock();
        Assert::LessThan(actual, expected);
    });

    TEST("MoveGen::Time: 1 - black", [file]() {
        chrono::nanoseconds expected(3215);
        auto actual = chrono::nanoseconds(RunTimeTest(fen, Enums::Colour::Black, count) / count);

        mtx.lock();
        std::println(file.get(), "1b:{}", actual);
        mtx.unlock();
        Assert::LessThan(actual, expected);
    });
}

void MoveGenTests()
{
    BreakMoveGen();
    InitialMoves();

    WhiteLondonTests();
    WhitePuzzleTests();

    BlackLondonTests();
    BlackPuzzleTests();

    CheckTests();
    DoubleCheckTests();
    MiscTests();

    TimeTests1();
}
