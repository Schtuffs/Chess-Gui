#include <string>

#include "TestSuite/TestSuite.h"

#include "GameManager.h"

#include <print>

static void GameManagerSuccess()
{
    TEST("GameManager::AllMoves: london - all successful", [](){
        std::string expected = "d2d4 d7d5 g1f3 g8f6 c1f4 b8c6 e2e3 e7e6 f3e5 f8b4 c2c3 b4a5";
        GameManager game(DEFAULT_FEN);
        std::vector<std::string> moves {"d2d4", "d7d5", "g1f3", "g8f6", "c1f4", "b8c6", "e2e3", "e7e6", "f3e5", "f8b4", "c2c3", "b4a5"};
        for (const auto& move : moves) {
            game.Update(move);
        }
        TestSuite::assertEqual(expected, game.AllMoves());
    });

}

static void GameManagerFailure()
{
    TEST("GameManager::AllMoves: london - one failure - end", [](){
        std::string expected = "d2d4 d7d5 g1f3 g8f6 c1f4 b8c6 e2e3 e7e6 f3e5 f8b4 c2c3 b4a5";
        GameManager game(DEFAULT_FEN);

        std::vector<std::string> moves {"d2d4", "d7d5", "g1f3", "g8f6", "c1f4", "b8c6", "e2e3", "e7e6", "f3e5", "f8b4", "c2c3", "b4a5", "a2a5"};
        for (const auto& move : moves) {
            game.Update(move);
        }

        TestSuite::assertEqual(expected, game.AllMoves());
    });

    TEST("GameManager::AllMoves: london - one failure - mid", [](){
        std::string expected = "d2d4 d7d5 g1f3 g8f6 c1f4 b8c6 e2e3 e7e6 f3e5 f8b4 c2c3 b4a5";
        GameManager game(DEFAULT_FEN);

        std::vector<std::string> moves {"d2d4", "d7d5", "g1f3", "g8f6", "c1f4", "a8a1", "b8c6", "e2e3", "e7e6", "f3e5", "f8b4", "c2c3", "b4a5"};
        for (const auto& move : moves) {
            game.Update(move);
        }

        TestSuite::assertEqual(expected, game.AllMoves());
    });

    TEST("GameManager::AllMoves: london - three failures - mid", [](){
        std::string expected = "d2d4 d7d5 g1f3 g8f6 c1f4 b8c6 e2e3 e7e6 f3e5 f8b4 c2c3 b4a5";
        GameManager game(DEFAULT_FEN);

        std::vector<std::string> moves {"d2d4", "e2e4", "d7d5", "g1f3", "g8f6", "e1g1", "c1f4", "b8c6", "e2e3", "e7e6", "f3e5", "f6d5", "f8b4", "c2c3", "b4a5"};
        for (const auto& move : moves) {
            game.Update(move);
        }

        TestSuite::assertEqual(expected, game.AllMoves());
    });
}

void GameManagerTests()
{
    GameManagerSuccess();
    GameManagerFailure();
}

