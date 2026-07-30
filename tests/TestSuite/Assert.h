#pragma once

#include <sstream>
#include <string>

#define TEST_FAIL(_text) throw std::string(_text)
#define TEST_SUCCESS

namespace Assert {
    // Checks if the value is true.
    void True(bool val);

    // Checks if the value is false.
    void False(bool val);

    // Checks if the value is equal.
    void Equal(const void* actual, const void* expected, int length);

    // Checks if the value is equal.
    template <typename T>
    void Equal(const T& actual, const T& expected) {
        if (actual == expected) {
            TEST_SUCCESS;
        }
        else {
            std::stringstream str;
            str << "<" << actual << "> is not equal to <" << expected << ">";
            TEST_FAIL(str.str());
        }
    }

    // Checks if the value is not equal.
    void NotEqual(const void* actual, const void* expected, int length);

    // Checks if the value is not equal.
    template <typename T>
    void NotEqual(const T& actual, const T& expected) {
        if (actual == expected) {
            std::stringstream str;
            str << "<" << actual << "> is equal to <" << expected << ">";
            TEST_FAIL(str.str());
        }
        TEST_SUCCESS;
    }

    // Checks if expected is greater than actual.
    void GreaterThan(const void* actual, const void* expected, int length);

    // Checks if expected is greater than actual.
    template <typename T>
    void GreaterThan(const T& actual, const T& expected) {
        if (actual <= expected) {
            std::stringstream str;
            str << "<" << actual << "> is not greater than <" << expected << ">";
            TEST_FAIL(str.str());
        }
        TEST_SUCCESS;
    }

    // Checks if expected is less than actual.
    void LessThan(const void* actual, const void* expected, int length);

    // Checks if expected is less than actual.
    template <typename T>
    void LessThan(const T& actual, const T& expected) {
        if (actual >= expected) {
            std::stringstream str;
            str << "<" << actual << "> is not less than <" << expected << ">";
            TEST_FAIL(str.str());
        }
        TEST_SUCCESS;
    }
}

