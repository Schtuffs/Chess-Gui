#include "Assert.h"

#include <cstring>

void Assert::True(bool value) {
    if (value == false) {
        TEST_FAIL("Assert::True: <false> is not <true>");
    }
    TEST_SUCCESS;
}

void Assert::False(bool value) {
    if (value == true) {
        TEST_FAIL("Assert::False: <true> is not <false>");
    }
    TEST_SUCCESS;
}

void Assert::Equal(const void* actual, const void* expected, int length) {
    if (memcmp(actual, expected, length) != 0) {
        std::stringstream str;
        str << "Assert::Equal: <" << actual << "> is not equal to <" << expected << ">";
        TEST_FAIL(str.str());
    }
    TEST_SUCCESS;
}

void Assert::NotEqual(const void* actual, const void* expected, int length) {
    if (memcmp(actual, expected, length) == 0) {
        std::stringstream str;
        str << "Assert::NotEqual: <" << actual << "> is equal to <" << expected << ">";
        TEST_FAIL(str.str());
    }
    TEST_SUCCESS;
}

void Assert::GreaterThan(const void* actual, const void* expected, int length) {
    if (memcmp(actual, expected, length) <= 0) {
        std::stringstream str;
        str << "Assert::GreaterThan: <" << actual << "> is not less than <" << expected << ">";
        TEST_FAIL(str.str());
    }
    TEST_SUCCESS;
}

void Assert::LessThan(const void* actual, const void* expected, int length) {
    if (memcmp(actual, expected, length) >= 0) {
        std::stringstream str;
        str << "Assert::LessThan: <" << actual << "> is not greater than <" << expected << ">";
        TEST_FAIL(str.str());
    }
    TEST_SUCCESS;
}


