#include <chrono>
#include <iostream>
#include <print>

#include "MoveGen/MoveGen.h"
#include "Types/BitBoard.h"
#include "Utils/Fen.h"

static std::chrono::nanoseconds RunTimeTest(std::string_view fen, u64 count)
{
    Position pos(fen);
    MoveList list;
    auto     start = std::chrono::steady_clock::now();
    for (u64 i = 0; i < count; i++) {
        list.size = 0;
        MoveGen::Generate(pos, list);
    }
    auto end = std::chrono::steady_clock::now();
    return (end - start) / count;
}

static std::chrono::nanoseconds LegalizationTest(std::string_view fen, u64 count)
{
    Position pos(fen);
    MoveList list;
    MoveGen::Generate(pos, list);
    list.Legalize(pos);
    auto start = std::chrono::steady_clock::now();

    for (u64 i = 0; i < count; i++) {
        list.Legalize(pos);
    }

    auto end = std::chrono::steady_clock::now();
    return (end - start) / count;
}

int main(int argc, char** argv)
{
    u64 count = 1'000;
    if (argc > 1) {
        try {
            count = std::stoull(argv[1]);
        } catch (...) {
            std::println("Invalid: {}", argv[1]);
        }
    }

    FILE* file = fopen("times1.log", "a");
    if (!file) {
        std::println(stderr, "ERROR: Could not open file.");
        return 1;
    }

    auto actual = RunTimeTest("8/1Q3p1k/4p1q1/7p/8/1B3p1P/P4PP1/6K1 w - - 0 34", count);
    std::println(file, "W1: {}", actual);

    actual = RunTimeTest("8/1Q3p1k/4p1q1/7p/8/1B3p1P/P4PP1/6K1 b - - 0 34", count);
    std::println(file, "B1: {}", actual);

    actual = LegalizationTest("8/1Q3p1k/4p1q1/7p/8/1B3p1P/P4PP1/6K1 w - - 0 34", count);
    std::println(file, "Lgl:{}", actual);

    fclose(file);

    return 0;
}
