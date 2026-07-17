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
 * @namespace TestSuite
 * @date 2025-11-27
 */
namespace TestSuite {

    void RunTests();
    
    // ----- Asserts -----

    /**
     * @brief Checks if the value is true.
     * @param val a hopefully true value.
     * @date 2025-11-27
     */
    // Checks if the value is true
    void assertTrue(bool val);
    
    /**
     * @brief Checks if the value is false.
     * @param val a hopefully false value.
     * @date 2025-11-27
     */
    void assertFalse(bool val);
    
    /**
     * @brief Checks if the expected value is equal to the actual value. Bytes length required.
     * @param expected the expected bytes to be receiving.
     * @param actual the actual bytes received.
     * @param length the length of the bytes.
     * @date 2025-11-27
     */
    void assertEqual(const void* expected, const void* actual, int length);
    
    /**
     * @brief Checks if the expected value is equal to the actual value. Type T must have operator ==.
     * @param expected the expected value.
     * @param actual the actual value.
     * @date 2025-11-27
     */
    template <typename T>
    void assertEqual(const T& expected, const T& actual) {
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
    void assertNotEqual(const void* expected, const void* actual, int length);
    
    /**
     * @brief Checks if the expected value is not equal to the actual value. Type T must have operator ==.
     * @param expected the expected value.
     * @param actual the actual value.
     * @date 2025-11-27
     */
    template <typename T>
    void assertNotEqual(const T& expected, const T& actual) {
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
    void add(const char* testName, std::function<void()> function);
};

#define TEST(__test_name, ...) TestSuite::add(__test_name, __VA_ARGS__)

