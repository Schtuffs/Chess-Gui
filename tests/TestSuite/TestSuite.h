#pragma once

#include <cstdint>
#include <functional>

/**
 * @brief Testing suite
 * @namespace TestSuite
 * @date 2025-11-27
 */
namespace TestSuite {

    // Prepare test suite with necessary data
    void Setup(int argc, char** argv);

    // Runs the tests
    uint64_t RunTests();

    // ----- Asserts -----

    /**
     * @brief Adds a test function to the list of tests.
     * @param testName The name of the test for easier debugging.
     * @param function the lambda function for testing.
     * @date 2025-11-27
     */
    void add(const char* testName, std::function<void()> function);
};

#define TEST(__test_name, ...) TestSuite::add(__test_name, __VA_ARGS__)

