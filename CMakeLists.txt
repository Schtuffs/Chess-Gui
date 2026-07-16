cmake_minimum_required(VERSION 3.22)
project(ChessGui C CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(INCLUDE     ${CMAKE_CURRENT_LIST_DIR}/include)
set(LIB         ${CMAKE_CURRENT_LIST_DIR}/lib)
set(SRC         ${CMAKE_CURRENT_LIST_DIR}/src)
set(THIRD_PARTY ${CMAKE_CURRENT_LIST_DIR}/third_party)

include(FetchContent)

FetchContent_Declare(
    raylib
    GIT_REPOSITORY https://github.com/raysan5/raylib
    GIT_TAG        6.0
)

FetchContent_MakeAvailable(raylib)

add_executable(${PROJECT_NAME}
    ${SRC}/Board.cpp
    ${SRC}/Button.cpp
    ${SRC}/Convert.cpp
    ${SRC}/Fen.cpp
    ${SRC}/GameManager.cpp
    ${SRC}/main.cpp
    ${SRC}/Menus.cpp
    ${SRC}/MoveGen.cpp
    ${SRC}/Piece.cpp
    ${SRC}/Renderer.cpp
    ${SRC}/Settings.cpp
    ${SRC}/Utils.cpp
    ${THIRD_PARTY}/raygui.c
)

target_include_directories(${PROJECT_NAME} PRIVATE ${INCLUDE})
target_include_directories(${PROJECT_NAME} SYSTEM PRIVATE ${THIRD_PARTY})

if(WIN32)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        WIN32_EXECUTABLE $<CONFIG:Release>
    )
endif()

target_compile_definitions(${PROJECT_NAME} PRIVATE $<$<CONFIG:Release>:UTILS_LOG_NONE>)

if (${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU" OR
    ${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang"
)
    target_link_libraries(${PROJECT_NAME} PRIVATE stdc++exp)
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Werror -Wpedantic)
endif()
target_link_libraries(${PROJECT_NAME} PRIVATE raylib)

add_subdirectory(tests)

