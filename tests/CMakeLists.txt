cmake_minimum_required(VERSION 3.22)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SRC ${CMAKE_CURRENT_LIST_DIR}/src)
set(SUITE ${CMAKE_CURRENT_LIST_DIR}/TestSuite)
set(INCLUDES ../include ../third_party)
set(SOURCES ../src)

file(GLOB ALL_SOURCE_FILES ${SOURCES}/*.cpp)
list(FILTER ALL_SOURCE_FILES EXCLUDE REGEX ".*/main\\.cpp$")

project(tests LANGUAGES C CXX)
add_subdirectory(TestSuite)

add_executable(${PROJECT_NAME}
    ${ALL_SOURCE_FILES}
    ../third_party/raygui.c
    tests.cpp
    BoardTests.cpp
    GameManagerTests.cpp
    MoveGenTests.cpp
)

if (${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU" OR
    ${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang"
)
    target_link_libraries(${PROJECT_NAME} PRIVATE stdc++exp)
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Werror -Wpedantic)
endif()

target_include_directories(${PROJECT_NAME} PRIVATE ${SUITE} ${INCLUDES})
target_link_libraries(${PROJECT_NAME} PRIVATE TestSuite)
target_link_libraries(${PROJECT_NAME} PRIVATE raylib)

