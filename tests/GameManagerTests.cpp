#include <string>

#include "TestSuite/TestSuite.h"

#include "GameManager.h"

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

    TEST("GameManager::Update: full - white promotion - queen", [](){
        std::string_view expected = "rnbqkbnQ/ppppp2p/8/8/8/8/PPPP1PPP/RNBQKBNR b KQq - 0 5";
        GameManager game(DEFAULT_FEN);
        std::vector<std::string> moves {"e2e4", "f7f5", "e4f5", "g7g5", "f5g6", "g8f6", "g6g7", "f6g8", "g7h8q"};

        for (const auto& move : moves) {
            game.Update(move);
        }
        std::string_view actual = game.Fen();

        TestSuite::assertEqual(expected, actual);
    });

    TEST("GameManager::Update: full - black promotion - bishop", [](){
        std::string_view expected = "rnbqkbnr/pppp1ppp/8/8/8/8/PP2PPPP/RbBQKBNR w KQkq - 0 5";
        GameManager game(DEFAULT_FEN);
        std::vector<std::string> moves {"d2d4", "e7e5", "b1c3", "e5d4", "c3b1", "d4d3", "g1f3", "d3c2", "f3g1", "c2b1b"};

        for (const auto& move : moves) {
            game.Update(move);
        }
        std::string_view actual = game.Fen();

        TestSuite::assertEqual(expected, actual);
    });

    TEST("GameManager::Update: partial - white promotion - queen", [](){
        std::string_view expected = "rnbqkbnR/ppppp2p/8/8/8/8/PPPP1PPP/RNBQKBNR b KQq - 0 5";
        GameManager game(DEFAULT_FEN);
        std::vector<std::string> moves {"e2e4", "f7f5", "e4f5", "g7g5", "f5g6", "g8f6", "g6g7", "f6g8", "g7h8", "h7"};

        for (const auto& move : moves) {
            game.Update(move);
        }
        std::string_view actual = game.Fen();

        TestSuite::assertEqual(expected, actual);
    });

    TEST("GameManager::Update: partial - black promotion - bishop", [](){
        std::string_view expected = "rnbqkbnr/pppp1ppp/8/8/8/8/PP2PPPP/RnBQKBNR w KQkq - 0 5";
        GameManager game(DEFAULT_FEN);
        std::vector<std::string> moves {"d2d4", "e7e5", "b1c3", "e5d4", "c3b1", "d4d3", "g1f3", "d3c2", "f3g1", "c2b1", "b4"};

        for (const auto& move : moves) {
            game.Update(move);
        }
        std::string_view actual = game.Fen();

        TestSuite::assertEqual(expected, actual);
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

