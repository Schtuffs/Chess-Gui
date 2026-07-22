#include "TestSuite.h"

#include <cstdint>
#include <iostream>
#include <print>
#include <string>
#include <sstream>

int sPasses = 0;
int sFails = 0;

std::vector<std::pair<const char*, std::function<void()>>> sTestFunctions;
static uint32_t sThreadCount = 1;

static std::vector<std::string> CreateArgList(int argc, char** argv)
{
    std::vector<std::string> args;
    args.reserve(argc - 1);
    for (int i = 1; i < argc; i++) {
        args.push_back(argv[i]);
    }
    return args;
}

static void ParseArgs(int argc, char** argv)
{
    constexpr const char* ARG_THREADS       = "-n";
    // constexpr const char* ARG_HELP_SHORT    = "-h";
    // constexpr const char* ARG_HELP_LONG     = "-help";
    // constexpr const char* ARG_HELP_QUESTION = "-?";

    std::vector args = CreateArgList(argc, argv);

    bool argRequiresNext = false;
    std::string prevArg;
    char firstPrint = '\n';
    for (const auto& arg : args) {
        if (argRequiresNext) {
            argRequiresNext = false;

            if (prevArg == ARG_THREADS) {
                try {
                    sThreadCount = std::stoi(arg);
                } catch (...) {
                    std::println("{}ERROR: Failed to parse threads from: {}", firstPrint, arg);
                    firstPrint = '\0';
                }
            }

            continue;
        }

        if (arg == ARG_THREADS) {
            argRequiresNext = true;
            prevArg = arg;
            continue;
        }

        std::println(stderr, "{}ERROR: Unknown argument: {}", firstPrint, arg);
        firstPrint = '\0';
    }
    std::println("\nRunning with {} threads", sThreadCount);
}

void TestSuite::add(const char* name, std::function<void()> function) {
    sTestFunctions.push_back({name, function});
}

int TestSuite::RunTests(int argc, char** argv) {
    ParseArgs(argc, argv);

    for (size_t i = 0; i < sTestFunctions.size(); i++) {
        try {
            sTestFunctions[i].second();
            sPasses++;
        } catch (std::string e) {
            if (sFails == 0) {
                std::println();
            }
            sFails++;
            std::println(stderr, "Test #{} ({}) failed! {}", i + 1, sTestFunctions[i].first, e.c_str());
        }
        catch (...) {
            if (sFails == 0) {
                std::println();
            }
            sFails++;
            std::println(stderr, "Test #{} ({}) failed! Uncaught exception!", i + 1, sTestFunctions[i].first);
        }
    }

    // Print data
    std::println("\nPasses: {}, Fails: {}, Success: {}%\n", sPasses, sFails, ((sPasses / (double)(sPasses + sFails)) * 100));

    // Exit program with the number of fails
    return (sFails);
}



// ----- Asserts -----

void TestSuite::assertTrue(bool value) {
    if (value == false) {
        TEST_FAIL("Expected <true>, received <false>");
    }
    TEST_SUCCESS;
}

void TestSuite::assertFalse(bool value) {
    if (value == true) {
        TEST_FAIL("Expected <false>, received <true>");
    }
    TEST_SUCCESS;
}

void TestSuite::assertEqual(const void* expected, const void* actual, int length) {
    if (memcmp(expected, actual, length) != 0) {
        std::stringstream str;
        str << "<" << expected << "> is not equal to <" << actual << ">";
        TEST_FAIL(str.str());
    }
    TEST_SUCCESS;
}

void TestSuite::assertNotEqual(const void* expected, const void* actual, int length) {
    if (memcmp(expected, actual, length) == 0) {
        std::stringstream str;
        str << "<" << expected << "> is equal to <" << actual << ">";
        TEST_FAIL(str.str());
    }
    TEST_SUCCESS;
}

