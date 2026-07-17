#include "TestSuite.h"

#include <iostream>
#include <print>
#include <string>
#include <sstream>

int sPasses = 0;
int sFails = 0;
std::vector<std::pair<const char*, std::function<void()>>> sTestFunctions;

void TestSuite::add(const char* name, std::function<void()> function) {
    sTestFunctions.push_back({name, function});
}

void TestSuite::RunTests() {
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
    exit(sFails);
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

