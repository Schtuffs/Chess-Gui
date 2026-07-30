#include "TestSuite.h"

#include <chrono>
#include <mutex>
#include <print>
#include <span>
#include <sstream>
#include <string>
#include <thread>
#include <utility>
#include <vector>

int sPasses = 0;
int sFails = 0;
bool sIsSuiteSetup = false;

std::vector<std::pair<const char*, std::function<void()>>> sTestFunctions;

// ----- Arguments -----

static std::vector<std::string> CreateArgList(int argc, char** argv)
{
    std::vector<std::string> args;
    args.reserve(argc - 1);
    for (int i = 1; i < argc; i++) {
        args.push_back(argv[i]);
    }
    return args;
}

static char s_firstPrint = '\n';
static uint32_t s_repeatRuns = 1;
static void AdjustCount(const std::string& arg)
{
    try {
        s_repeatRuns = std::stoi(arg);
        if (s_repeatRuns < 1) {
            s_repeatRuns = 1;
            std::println(stderr, "{}ERROR: Invalid run count given: {}", s_firstPrint, arg);
            s_firstPrint = '\0';
        }
    } catch (...) {
        std::println(stderr, "{}ERROR: Failed to parse threads from: {}", s_firstPrint, arg);
        s_firstPrint = '\0';
    }
}

static uint32_t s_syncJobs = std::thread::hardware_concurrency();
static void AdjustJobs(const std::string& arg)
{
    try {
        s_syncJobs = std::stoi(arg);
        if (s_syncJobs < 1) {
            s_syncJobs = 1;
            std::println(stderr, "{}ERROR: Invalid run count given: {}", s_firstPrint, arg);
            s_firstPrint = '\0';
        }
    } catch (...) {
        std::println(stderr, "{}ERROR: Failed to parse threads from: {}", s_firstPrint, arg);
        s_firstPrint = '\0';
    }
}

static std::vector<std::string> s_testNameRequirements;
static bool s_nameAddAll = true;
static void AdjustName(const std::string& arg)
{
    s_testNameRequirements.clear();
    s_nameAddAll = false;

    // Its either an empty string or a valid name
    uint64_t index = arg.find_first_of("*");
    if (index == std::string::npos) {
        if (arg == "") {
            s_nameAddAll = true;
            s_testNameRequirements.push_back("*");
        } else {
            s_testNameRequirements.push_back(arg);
        }
        return;
    }

    if (arg[0] == '*') {
        s_testNameRequirements.push_back("*");
    }

    uint64_t prev = 0;
    uint64_t safety = 50;
    while ((index = arg.find("*", prev)) != std::string::npos && safety != 0) {
        if (index != prev) {
            s_testNameRequirements.push_back(arg.substr(prev, index - prev));
            if (arg[prev + (index - prev)] == '*') {
                s_testNameRequirements.push_back("*");
            }
        }
        prev = index + 1;
        safety--;
    }

    if (safety == 0) {
        std::println(stderr, "ERROR: Safety triggered on test name parse: {}", arg);
    }

    index = arg.find_last_of("*");
    if (index != arg.size() - 1) {
        s_testNameRequirements.push_back(arg.substr(index + 1));
    }
}

static void ParseArgs(int argc, char** argv)
{
    std::array<std::pair<const char*, std::function<void(const std::string&)>>, 6> ARG_LIST = {{
        {"-c", AdjustCount},
        {"--count", AdjustCount},
        {"-j", AdjustJobs},
        {"--jobs", AdjustJobs},
        {"-n", AdjustName},
        {"--name", AdjustName},
    }};

    std::vector args = CreateArgList(argc, argv);

    bool validArgFound = false;
    std::pair<const char*, std::function<void(const std::string&)>> prevArg;
    for (const auto& arg : args) {
        if (validArgFound) {
            validArgFound = false;
            prevArg.second(arg);
            continue;
        }
        // Iterate over all entries
        auto it = std::find_if(
            ARG_LIST.begin(), ARG_LIST.end(),
            [&arg](const std::pair<const char*, std::function<void(const std::string&)>>& list) {
                return (arg == list.first);
            });

        // If found call function on next run
        if (it != ARG_LIST.end()) {
            prevArg = *it;
            validArgFound = true;
            continue;
        }

        std::println(stderr, "{}ERROR: Unknown argument: {}", s_firstPrint, arg);
        s_firstPrint = '\0';
    }
    std::println("\nRunning with {} threads", s_syncJobs);
}

void TestSuite::Setup(int argc, char** argv)
{
    ParseArgs(argc, argv);
    sIsSuiteSetup = true;
}

// ----- Adding Tests -----

static bool IsValidTestName(std::string_view name)
{
    if (s_nameAddAll) {
        return true;
    }

    if (s_testNameRequirements[0] != "*" && !name.starts_with(s_testNameRequirements[0])) {
        return false;
    }

    uint64_t index = 0;
    bool canContinue = true;
    for (uint64_t i = 0; i < s_testNameRequirements.size(); i++) {
        bool finalEndsWith = (i == s_testNameRequirements.size() - 1);
        const auto& requirement = s_testNameRequirements[i];
        if (requirement == "*") {
            canContinue = true;
            continue;
        }

        if (!canContinue) {
            return false;
        }

        if (finalEndsWith && !name.ends_with(requirement)) {
            return false;
        }

        index = name.find(requirement, index);
        if (index == std::string_view::npos) {
            return false;
        }

        canContinue = false;
    }

    return true;
}

void TestSuite::add(const char* name, std::function<void()> function)
{
    if (!sIsSuiteSetup) {
        std::println(stderr, "\nERROR: Must call TestSuite::Setup(int argc, char** argv) first.\n");
        exit(1);
    }

    if (!IsValidTestName(name)) {
        return;
    }

    sTestFunctions.push_back({name, function});
}

// ----- Running Tests -----

static bool RunTest(const std::pair<const char*, std::function<void()>>& test)
{
    static std::mutex mtx;
    try {
        test.second();
        return true;
    } catch (std::string e) {
        mtx.lock();
        std::println(stderr, "{}Test ({}) failed! {}", s_firstPrint, test.first, e.c_str());
        s_firstPrint = '\0';
        mtx.unlock();
    } catch (...) {
        mtx.lock();
        std::println(stderr, "{}Test ({}) failed! Uncaught exception!", s_firstPrint, test.first);
        s_firstPrint = '\0';
        mtx.unlock();
    }
    return false;
}

static std::pair<uint64_t, uint64_t>
RunTests(std::span<std::pair<const char*, std::function<void()>>> tests)
{
    std::pair<uint64_t, uint64_t> results = {};
    for (const auto& test : tests) {
        if (RunTest(test)) {
            results.first++;
        } else {
            results.second++;
        }
    }
    return results;
}

static void TestLoop(uint64_t runTimes,
                     std::span<std::pair<const char*, std::function<void()>>> tests)
{
    static std::mutex mtx;
    uint64_t pass = 0, fail = 0;

    for (uint64_t i = 0; i < runTimes; i++) {
        auto results = ::RunTests(tests);
        pass += results.first;
        fail += results.second;
    }

    mtx.lock();
    sPasses += pass;
    sFails += fail;
    mtx.unlock();
}

uint64_t TestSuite::RunTests()
{
    s_firstPrint = '\n';

    const auto& start = std::chrono::system_clock::now();

    if (s_syncJobs == 1) {
        TestLoop(s_repeatRuns, sTestFunctions);
    } else {
        std::vector<std::jthread> threads;
        threads.reserve(s_syncJobs);
        uint64_t listPtr = 0;

        for (uint64_t i = 0; i < s_syncJobs; i++) {
            uint64_t functionCount = (sTestFunctions.size() - listPtr) / (s_syncJobs - i);
            threads.emplace_back([functionCount, listPtr]() {
                TestLoop(s_repeatRuns, std::span(sTestFunctions.begin() + listPtr, functionCount));
            });
            listPtr += functionCount;
        }
    }

    const auto& end = std::chrono::system_clock::now();
    const auto& duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    // Print data
    FILE* out = stdout;
    std::println(out);
    std::println(out, "Total tests: {}, Run {}x each", sTestFunctions.size(), s_repeatRuns);
    std::println(out, "Passes: {}, Fails: {}, Success: {}%", sPasses, sFails,
                 ((sPasses / (double)(sPasses + sFails)) * 100));
    std::println(out, "Total test runtime: {}", duration);
    std::println(out);

    // Exit program with the number of fails
    return (sFails);
}
