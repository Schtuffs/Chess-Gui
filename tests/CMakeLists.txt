cmake_minimum_required(VERSION 3.25)

# Setup variables
set(CMAKE_CXX_STANDARD 23)
set(SRC ${CMAKE_CURRENT_LIST_DIR}/src)
set(SUITE ${CMAKE_CURRENT_LIST_DIR}/TestSuite)
set(INCLUDES ../include)
set(SOURCES ../src)

# Get sources from main system and remove its main file
file(GLOB ALL_SOURCE_FILES ${SOURCES}/*.cpp)
list(REMOVE_ITEM ALL_SOURCE_FILES ${SOURCES}/main.cpp)
message("List: ${ALL_SOURCE_FILES}")

project(Tests CXX)
add_subdirectory(TestSuite)

add_executable(${PROJECT_NAME}
    ${ALL_SOURCE_FILES}
    tests.cpp
)

target_compile_definitions(${PROJECT_NAME} PRIVATE $<$<CONFIG:Release>:UTILS_LOG_NONE>)

if (${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU" OR
    ${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang"
)
    target_link_libraries(${PROJECT_NAME} PRIVATE stdc++exp)
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Werror -Wpedantic)
endif()

target_include_directories(${PROJECT_NAME} PRIVATE ${SUITE} ${INCLUDES})
target_link_libraries(${PROJECT_NAME} PRIVATE TestSuite)

