#include "TestSuite/Assert.h"
#include "TestSuite/TestSuite.h"

#include <chrono>
#include <iostream>
#include <memory>
#include <mutex>
#include <print>

#include "Board.h"
#include "Convert.h"
#include "MoveGen.h"

static void BreakMoveGen()
{
    TEST("MoveGen::Generate: index out of bounds", []() {
        BitBoard expected = MoveGen::INVALID;
        Piece    pieces[64];
        MoveGen  gen;

        gen.Generate(pieces, 64, 12);
        BitBoard actual = gen.GetMoves();

        Assert::Equal(expected, actual);
    });
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
        gen.Generate(b.Pieces(), 4, b.Castling());
        BitBoard actual = gen.GetMoves();

        Assert::Equal(expected, actual);
    });

    TEST("MoveGen::Generate: london piece movements - white queen", [&fen]() {
        Board    b(fen);
        BitBoard expected = 0x00'00'00'80'41'22'1c'0c;

        MoveGen gen;
        gen.Generate(b.Pieces(), 3, b.Castling());
        BitBoard actual = gen.GetMoves();

        Assert::Equal(expected, actual);
    });

    TEST("MoveGen::Generate: london piece movements - white pinned pawn e3", [&fen]() {
        Board    b(fen);
        BitBoard expected = 0x00'00'00'00'00'04'00'00;

        MoveGen gen;
        gen.Generate(b.Pieces(), 18, b.Castling());
        BitBoard actual = gen.GetMoves();

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
        gen.Generate(b.Pieces(), 48, b.Castling());
        BitBoard actual = gen.GetMoves();

        Assert::Equal(expected, actual);
    });

    TEST("MoveGen::Generate: puzzle king in check - move light bishop", [&fen]() {
        BitBoard    expected = 0x00'00'00'00'00'02'00'00;
        const Board b(fen);

        MoveGen gen;
        gen.Generate(b.Pieces(), 17, b.Castling());
        BitBoard actual = gen.GetMoves();

        Assert::Equal(expected, actual);
    });

    TEST("MoveGen::Generate: puzzle king in check - move king", [&fen]() {
        BitBoard    expected = 0x00'00'00'00'00'00'40'40;
        const Board b(fen);

        MoveGen gen;
        gen.Generate(b.Pieces(), 6, b.Castling());
        BitBoard actual = gen.GetMoves();

        Assert::Equal(expected, actual);
    });

    TEST("MoveGen::Generate: puzzle king in check - move rook", [&fen]() {
        BitBoard    expected = 0x00'00'00'00'00'00'20'20;
        const Board b(fen);

        MoveGen gen;
        gen.Generate(b.Pieces(), 5, b.Castling());
        BitBoard actual = gen.GetMoves();

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
        gen.Generate(b.Pieces(), 62, b.Castling());
        BitBoard actual = gen.GetMoves();

        Assert::Equal(expected, actual);
    });

    TEST("MoveGen::Generate: london piece movements - black pawn g7", [&fen]() {
        BitBoard    expected = 0x00'40'00'00'00'00'00'00;
        const Board b(fen);

        MoveGen gen;
        gen.Generate(b.Pieces(), 54, b.Castling());
        BitBoard actual = gen.GetMoves();

        Assert::Equal(expected, actual);
    });

    TEST("MoveGen::Generate: london piece movements - black pinned pawn e3", [&fen]() {
        BitBoard    expected = 0x00'00'00'00'00'04'00'00;
        const Board b(fen);

        MoveGen gen;
        gen.Generate(b.Pieces(), 18, b.Castling());
        BitBoard actual = gen.GetMoves();

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
        gen.Generate(b.Pieces(), 20, b.Castling());
        BitBoard actual = gen.GetMoves();

        Assert::Equal(expected, actual);
    });

    TEST("MoveGen::Generate: knight check - pawn", []() {
        BitBoard expected = 0x00'10'00'00'00'00'00'00;
        Board    b("rnbq1r2/ppp1pp1k/3p2p1/6N1/2P5/4b3/PP3PPP/R1BQKB1R b KQ - 1 9");

        MoveGen gen;
        gen.Generate(b.Pieces(), 52, b.Castling());
        BitBoard actual = gen.GetMoves();

        Assert::Equal(expected, actual);
    });

    TEST("MoveGen::Generate: knight check - king", []() {
        BitBoard expected = 0xc0'c0'80'00'00'00'00'00;
        Board    b("rnbq1r2/ppp1pp1k/3p2p1/6N1/2P5/4b3/PP3PPP/R1BQKB1R b KQ - 1 9");

        MoveGen gen;
        gen.Generate(b.Pieces(), 55, b.Castling());
        BitBoard actual = gen.GetMoves();

        Assert::Equal(expected, actual);
    });

    TEST("MoveGen::Generate: pawn check - king", []() {
        BitBoard expected = 0xc0'c0'c0'00'00'00'00'00;
        Board    b("rnbq1r2/ppp1pp1k/3p2P1/8/2P5/4b3/PP3PPP/R1BQKB1R b KQ - 1 9");

        MoveGen gen;
        gen.Generate(b.Pieces(), 55, b.Castling());
        BitBoard actual = gen.GetMoves();

        Assert::Equal(expected, actual);
    });

    TEST("MoveGen::Generate: pawn check - pawn", []() {
        BitBoard expected = 0x00'20'40'00'00'00'00'00;
        Board    b("rnbq1r2/ppp1pp1k/3p2P1/8/2P5/4b3/PP3PPP/R1BQKB1R b KQ - 1 9");

        MoveGen gen;
        gen.Generate(b.Pieces(), 53, b.Castling());
        BitBoard actual = gen.GetMoves();

        Assert::Equal(expected, actual);
    });

    TEST("MoveGen::Generate: pawn check defended - king", []() {
        BitBoard expected = 0xc0'c0'80'00'00'00'00'00;
        Board    b("rnbq1r2/ppp1pp1k/3p2P1/8/2P2N2/4b3/PP3PPP/R1BQKB1R b KQ - 1 9");

        MoveGen gen;
        gen.Generate(b.Pieces(), 55, b.Castling());
        BitBoard actual = gen.GetMoves();

        Assert::Equal(expected, actual);
    });
}

static void DoubleCheckTests()
{
    TEST("MoveGen::Generate: double check - king", []() {
        BitBoard expected = 0xc0'c0'c0'00'00'00'00'00;
        Board    b("rnbq1r2/ppp1pp1k/3p2P1/6N1/2P5/4b3/PP3PPP/R1BQKB1R b KQ - 0 9");

        MoveGen gen;
        gen.Generate(b.Pieces(), 55, b.Castling());
        BitBoard actual = gen.GetMoves();

        Assert::Equal(actual, expected);
    });

    TEST("MoveGen::Generate: double check defended pawn - king", []() {
        BitBoard expected = 0xc0'c0'80'00'00'00'00'00;
        Board    b("rnbq1r2/ppp1pp1k/3p2P1/6N1/2P4N/4b3/PP3PPP/R1BQKB1R b KQ - 0 9");

        MoveGen gen;
        gen.Generate(b.Pieces(), 55, b.Castling());
        BitBoard actual = gen.GetMoves();

        Assert::Equal(actual, expected);
    });

    TEST("MoveGen::Generate: double check - bishop", []() {
        BitBoard expected = 0x00'00'00'00'00'10'00'00;
        Board    b("rnbq1r2/ppp1pp1k/3p2P1/6N1/2P4N/4b3/PP3PPP/R1BQKB1R b KQ - 0 9");

        MoveGen gen;
        gen.Generate(b.Pieces(), 20, b.Castling());
        BitBoard actual = gen.GetMoves();

        Assert::Equal(actual, expected);
    });

    TEST("MoveGen::Generate: double check - pawn", []() {
        BitBoard expected = 0x00'20'00'00'00'00'00'00;
        Board    b("rnbq1r2/ppp1pp1k/3p2P1/6N1/2P4N/4b3/PP3PPP/R1BQKB1R b KQ - 0 9");

        MoveGen gen;
        gen.Generate(b.Pieces(), 53, b.Castling());
        BitBoard actual = gen.GetMoves();

        Assert::Equal(actual, expected);
    });
}

static void MiscTests()
{
    TEST("MoveGen::Generate: kings indian piece movements - move white on black turn", []() {
        BitBoard expected = (u64)(0x00'00'00'00'02'02'02'00);
        Board    b("r1bq1rk1/ppp2ppB/2n1pn2/b2pN3/3P1B2/2P1P3/PP3PPP/RN1QK2R b KQ - 0 7");

        MoveGen gen;
        gen.Generate(b.Pieces(), 9, b.Castling());
        BitBoard actual = gen.GetMoves();

        Assert::Equal(expected, actual);
    });

    TEST("MoveGen::Generate: invalid piece", []() {
        BitBoard expected = MoveGen::INVALID;
        Board    b(DEFAULT_FEN);

        MoveGen gen;
        gen.Generate(b.Pieces(), 34, b.Castling());
        BitBoard actual = gen.GetMoves();

        Assert::Equal(expected, actual);
    });

    TEST("MoveGen::Generate: en passant - from fen", []() {
        BitBoard expected = 0x00'00'18'08'00'00'00'00;
        Board    b("rnbqkbnr/pppp1ppp/8/3Pp3/8/8/PPP1PPPP/RNBQKBNR w KQkq e6 0 2");

        MoveGen gen;
        gen.Generate(b.Pieces(), 35, b.Castling());
        BitBoard actual = gen.GetMoves();

        Assert::Equal(expected, actual);
    });

    TEST("MoveGen::Generate: en passant - make moves", []() {
        BitBoard                 expected = 0x00'00'30'10'00'00'00'00;
        Board                    b(DEFAULT_FEN);
        std::vector<std::string> moves = {"e2e4", "d7d5", "e4e5", "f7f5"};
        for (const auto& move : moves) {
            Assert::True(b.MakeMove(move));
        }
        MoveGen gen;
        gen.Generate(b.Pieces(), 36, b.Castling());
        BitBoard actual = gen.GetMoves();

        Assert::Equal(expected, actual);
    });

    TEST("MoveGen::Generate: en passant - ensure pawn gone", []() {
        BitBoard                 expected = 0x00'00'00'00'00'00'00'00;
        Board                    b(DEFAULT_FEN);
        std::vector<std::string> moves = {"e2e4", "d7d5", "e4e5", "f7f5", "e5f6"};
        for (const auto& move : moves) {
            Assert::True(b.MakeMove(move));
        }
        MoveGen gen;
        gen.Generate(b.Pieces(), 37, b.Castling());
        BitBoard actual = gen.GetMoves();

        Assert::Equal(expected, actual);
    });
}

//

static std::chrono::nanoseconds RunTimeTest(std::string_view fen, Index index, u64 count)
{
    std::chrono::nanoseconds totalTime = {};
    for (u64 i = 0; i < count; i++) {
        Board   b(fen);
        MoveGen gen;

        auto start = std::chrono::steady_clock::now();
        gen.Generate(b.Pieces(), index, 0);
        auto end = std::chrono::steady_clock::now();

        auto delta = std::chrono::nanoseconds(end - start);
        totalTime += delta;
    }
    return totalTime;
}

static void TimeTests1()
{
    constexpr std::string_view fen = "8/1Q3p1k/4p1q1/7p/8/1B3p1P/P4PP1/6K1 b - - 0 34";
    constexpr static u64       count = 10000;
    static std::mutex          mtx;
    std::shared_ptr<FILE>      file(fopen("TimeTests1.log", "a"), fclose);

    TEST("MoveGen::Time: 1 - index 21", [file]() {
        std::chrono::nanoseconds expected(3000);
        auto actual = std::chrono::nanoseconds(RunTimeTest(fen, 21, count) / count);

        mtx.lock();
        std::println(file.get(), "Index 21: {}", actual);
        mtx.unlock();
        Assert::LessThan(actual, expected);
    });

    TEST("MoveGen::Time: 1 - index 39", [file]() {
        std::chrono::nanoseconds expected(3000);
        auto actual = std::chrono::nanoseconds(RunTimeTest(fen, 39, count) / count);

        mtx.lock();
        std::println(file.get(), "Index 39: {}", actual);
        mtx.unlock();
        Assert::LessThan(actual, expected);
    });

    TEST("MoveGen::Time: 1 - index 44", [file]() {
        std::chrono::nanoseconds expected(3000);
        auto actual = std::chrono::nanoseconds(RunTimeTest(fen, 44, count) / count);

        mtx.lock();
        std::println(file.get(), "Index 44: {}", actual);
        mtx.unlock();
        Assert::LessThan(actual, expected);
    });

    TEST("MoveGen::Time: 1 - index 46", [file]() {
        std::chrono::nanoseconds expected(3100);
        auto actual = std::chrono::nanoseconds(RunTimeTest(fen, 46, count) / count);

        mtx.lock();
        std::println(file.get(), "Index 46: {}", actual);
        mtx.unlock();
        Assert::LessThan(actual, expected);
    });

    TEST("MoveGen::Time: 1 - index 53", [file]() {
        std::chrono::nanoseconds expected(3000);
        auto actual = std::chrono::nanoseconds(RunTimeTest(fen, 53, count) / count);

        mtx.lock();
        std::println(file.get(), "Index 53: {}", actual);
        mtx.unlock();
        Assert::LessThan(actual, expected);
    });

    TEST("MoveGen::Time: 1 - index 55", [file]() {
        std::chrono::nanoseconds expected(3000);
        auto actual = std::chrono::nanoseconds(RunTimeTest(fen, 55, count) / count);

        mtx.lock();
        std::println(file.get(), "Index 55: {}", actual);
        mtx.unlock();
        Assert::LessThan(actual, expected);
    });
}

void MoveGenTests()
{
    BreakMoveGen();

    WhiteLondonTests();
    WhitePuzzleTests();

    BlackLondonTests();
    BlackPuzzleTests();

    CheckTests();
    DoubleCheckTests();
    MiscTests();

    TimeTests1();
}
