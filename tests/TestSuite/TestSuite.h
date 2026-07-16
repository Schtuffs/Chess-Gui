#pragma once

#include <cstring>
#include <functional>
#include <iostream>
#include <sstream>
#include <utility>
#include <vector>

#define TEST_FAIL(_text) throw std::string(_text)
#define TEST_SUCCESS

/**
 * @brief Testing suite
 * @class TestSuite
 * @date 2025-11-27
 */
class TestSuite {
public:
    
    // ----- Asserts -----

    /**
     * @brief Checks if the value is true.
     * @param val a hopefully true value.
     * @date 2025-11-27
     */
    // Checks if the value is true
    static void assertTrue(bool val);
    
    /**
     * @brief Checks if the value is false.
     * @param val a hopefully false value.
     * @date 2025-11-27
     */
    static void assertFalse(bool val);
    
    /**
     * @brief Checks if the expected value is equal to the actual value. Bytes length required.
     * @param expected the expected bytes to be receiving.
     * @param actual the actual bytes received.
     * @param length the length of the bytes.
     * @date 2025-11-27
     */
    static void assertEqual(const void* expected, const void* actual, int length);
    
    /**
     * @brief Checks if the expected value is equal to the actual value. Type T must have operator ==.
     * @param expected the expected value.
     * @param actual the actual value.
     * @date 2025-11-27
     */
    template <typename T>
    static void assertEqual(const T& expected, const T& actual) {
        if (expected == actual) {
            TEST_SUCCESS;
        }
        else {
            std::stringstream str;
            str << "<" << expected << "> is not equal to <" << actual << ">";
            TEST_FAIL(str.str());
        }
    }
    
    /**
     * @brief Checks if the expected value is not equal to the actual value. Bytes length required.
     * @param expected the expected bytes to be receiving.
     * @param actual the actual bytes received.
     * @param length the length of the bytes.
     * @date 2026-07-15
     */
    static void assertNotEqual(const void* expected, const void* actual, int length);
    
    /**
     * @brief Checks if the expected value is not equal to the actual value. Type T must have operator ==.
     * @param expected the expected value.
     * @param actual the actual value.
     * @date 2025-11-27
     */
    template <typename T>
    static void assertNotEqual(const T& expected, const T& actual) {
        if (expected == actual) {
            std::stringstream str;
            str << "<" << expected << "> is equal to <" << actual << ">";
            TEST_FAIL(str.str());
        }
        TEST_SUCCESS;
    }
    
    /**
     * @brief Adds a test function to the list of tests.
     * @param testName The name of the test for easier debugging.
     * @param function the lambda function for testing.
     * @date 2025-11-27
     */
    static void add(const char* testName, std::function<void()> function);

private:
    static std::vector<std::pair<const char*, std::function<void()>>> sTestFunctions;
    static int sPasses, sFails;
    static TestSuite sSuite;

    // Inaccessible functions
    TestSuite();
    static void invoke();
    ~TestSuite();
};

#define TEST(__test_name, ...) TestSuite::add(__test_name, __VA_ARGS__)

