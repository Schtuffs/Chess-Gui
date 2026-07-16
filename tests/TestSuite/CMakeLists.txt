cmake_minimum_required(VERSION 3.22)

set(CMAKE_CXX_STANDARD 23)
project(TestSuite LANGUAGES C CXX)

add_library(${PROJECT_NAME} STATIC
    TestSuite.cpp
)

if (${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU" OR
    ${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang"
)
    target_link_libraries(${PROJECT_NAME} PRIVATE stdc++exp)
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Werror -Wpedantic)
endif()

