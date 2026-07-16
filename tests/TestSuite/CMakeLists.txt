cmake_minimum_required(VERSION 3.25)

set(CMAKE_CXX_STANDARD 23)
project(TestSuite CXX)

add_library(${PROJECT_NAME} STATIC
    TestSuite.cpp
)

target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic -Werror)
target_link_libraries(${PROJECT_NAME} PRIVATE stdc++exp)

