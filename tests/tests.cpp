#include "TestSuite.h"

int main(void) {
    TEST("Example", [](){
        const char* text = "example";
        TestSuite::assertEqual(text, text, sizeof(text));
    });
    
    return 0;
}

