#include <print>
#include <sstream>
#include <string>

#include "TestSuite/Assert.h"
#include "TestSuite/TestSuite.h"

#include "State/GameManager.h"
#include "Utils/Convert.h"
#include "Utils/Fen.h"

static void Promotion()
{
    TEST("GameManager::Update: full - white promotion - queen", []() {
        std::string_view expected = "rnbqkbnQ/ppppp2p/8/8/8/8/PPPP1PPP/RNBQKBNR b KQq - 0 5";
        GameManager      game(Fen::DEFAULT);
        std::vector<std::string> moves{"e2e4", "f7f5", "e4f5", "g7g5", "f5g6",
                                       "g8f6", "g6g7", "f6g8", "g7h8q"};

        for (const auto& move : moves) {
            game.Update(Convert::StrToMove(move, game.Player()));
        }
        std::string_view actual = game.Fen();

        Assert::Equal(actual, expected);
    });

    TEST("GameManager::Update: full - black promotion - bishop", []() {
        std::string_view expected = "rnbqkbnr/pppp1ppp/8/8/8/8/PP2PPPP/RbBQKBNR w KQkq - 0 5";
        GameManager      game(Fen::DEFAULT);
        std::vector<std::string> moves{"d2d4", "e7e5", "b1c3", "e5d4", "c3b1",
                                       "d4d3", "g1f3", "d3c2", "f3g1", "c2b1b"};

        for (const auto& move : moves) {
            game.Update(Convert::StrToMove(move, game.Player()));
        }
        std::string_view actual = game.Fen();

        Assert::Equal(actual, expected);
    });

    TEST("GameManager::Update: partial - white promotion - queen", []() {
        std::string_view expected = "rnbqkbnQ/ppppp2p/8/8/8/8/PPPP1PPP/RNBQKBNR b KQq - 0 5";
        GameManager      game(Fen::DEFAULT);
        std::vector<std::string> moves{"e2e4", "f7f5", "e4f5", "g7g5", "f5g6",
                                       "g8f6", "g6g7", "f6g8", "g7h8"};

        for (const auto& move : moves) {
            game.Update(Convert::StrToMove(move, game.Player()));
        }
        game.Promote(QUEEN);
        std::string_view actual = game.Fen();

        Assert::Equal(actual, expected);
    });

    // TEST("GameManager::Update: partial - black promotion - bishop", []() {
    //     std::string_view expected = "rnbqkbnr/pppp1ppp/8/8/8/8/PP2PPPP/RbBQKBNR w KQkq - 0 5";
    //     GameManager      game(Fen::DEFAULT);
    //     std::vector<std::string> moves{"d2d4", "e7e5", "b1c3", "e5d4", "c3b1", "d4d3",
    //                                    "g1f3", "d3c2", "f3g1", "c2b1", "b3"};

    //     for (const auto& move : moves) {
    //         game.Update(Convert::StrToMove(move, game.Player()));
    //     }
    //     std::string_view actual = game.Fen();

    //     Assert::Equal(actual, expected);
    // });
}

static void Castling()
{
    TEST("GameManager::Update: white short castle", []() {
        std::string_view expected = "rnbqk2r/ppppppbp/5np1/8/8/5NP1/PPPPPPBP/RNBQ1RK1 b kq - 3 4";
        GameManager      game("rnbqk2r/ppppppbp/5np1/8/8/5NP1/PPPPPPBP/RNBQK2R w KQkq - 2 3");

        game.Update(Convert::StrToMove("O-O", game.Player()));
        std::string_view actual = game.Fen();

        Assert::Equal(actual, expected);
    });

    TEST("GameManager::Update: black short castle", []() {
        std::string_view expected = "rnbq1rk1/ppppppbp/5np1/8/8/5NP1/PPPPPPBP/RNBQ1RK1 w - - 4 4";
        GameManager      game("rnbqk2r/ppppppbp/5np1/8/8/5NP1/PPPPPPBP/RNBQ1RK1 b kq - 3 4");

        game.Update(Convert::StrToMove("O-O", game.Player()));
        std::string_view actual = game.Fen();

        Assert::Equal(actual, expected);
    });

    TEST("GameManager::Update: white long castle", []() {
        std::string_view expected =
            "r3kbnr/ppp1pppp/2nqb3/3p4/3P4/2NQB3/PPP1PPPP/2KR1BNR b kq - 7 5";
        GameManager game("r3kbnr/ppp1pppp/2nqb3/3p4/3P4/2NQB3/PPP1PPPP/R3KBNR w KQkq - 6 4");

        game.Update(Convert::StrToMove("O-O-O", game.Player()));
        std::string_view actual = game.Fen();

        Assert::Equal(actual, expected);
    });

    TEST("GameManager::Update: black long castle", []() {
        std::string_view expected =
            "2kr1bnr/ppp1pppp/2nqb3/3p4/3P4/2NQB3/PPP1PPPP/2KR1BNR w - - 8 5";
        GameManager game("r3kbnr/ppp1pppp/2nqb3/3p4/3P4/2NQB3/PPP1PPPP/2KR1BNR b kq - 7 5");

        game.Update(Convert::StrToMove("O-O-O", game.Player()));
        std::string_view actual = game.Fen();

        Assert::Equal(actual, expected);
    });
}

static void GameManagerSuccess()
{
    TEST("GameManager::AllMoves: london - all successful", []() {
        std::string expected =
            "r1bqk2r/ppp2ppp/2n1pn2/b2pN3/3P1B2/2P1P3/PP3PPP/RN1QKB1R w KQkq - 1 6";
        GameManager              game(Fen::DEFAULT);
        std::vector<std::string> moves{"d2d4", "d7d5", "g1f3", "g8f6", "c1f4", "b8c6",
                                       "e2e3", "e7e6", "f3e5", "f8b4", "c2c3", "b4a5"};

        for (const auto& move : moves) {
            game.Update(Convert::StrToMove(move, game.Player()));
        }

        Assert::Equal<std::string>(game.Fen().data(), expected);
    });

    TEST("GameManager::Update: checkmate", []() {
        GameManager game("k7/2Q5/2K5/8/8/8/8/8 w - - 12 34");

        game.Update(Convert::StrToMove("c7b7", game.Player()));

        Assert::True(game.InCheckmate());
    });

    TEST("GameManager::Update: stalemate", []() {
        GameManager game("k7/3Q4/2K5/8/8/8/8/8 w - - 12 34");

        game.Update(Convert::StrToMove("d7c7", game.Player()));

        Assert::True(game.InStalemate());
    });

    TEST("GameManager::Update: random knight move", []() {
        std::string_view expected = "8/8/8/8/5p1p/1Nk5/p6K/8 w - - 9 175";
        GameManager      game(Fen::DEFAULT);

        std::stringstream ss(
            "e2e4 e7e5 g1f3 b8c6 f1b5 a7a6 b5a4 g8f6 e1g1 b7b5 a4b3 c8b7 f1e1 f8c5 c2c3 c5b6 d2d4 "
            "d7d6 c1e3 e8g8 b1d2 h7h6 d4e5 c6e5 f3e5 d6e5 e3b6 c7b6 f2f3 f6h5 d2f1 d8g5 g2g3 a8d8 "
            "d1c2 b7c8 c2g2 g7g6 a1d1 g8g7 a2a3 g5e7 g2f2 c8e6 b3e6 e7e6 d1d2 d8d2 f2d2 e6f6 e1e3 "
            "f8d8 e3d3 d8d3 d2d3 f6e6 f1e3 e6a2 d3d2 h5f6 e3d5 a2c4 d5b6 c4c5 d2f2 c5d6 c3c4 d6d1 "
            "g1g2 g6g5 g3g4 h6h5 f2e3 d1c2 g2g3 g7g6 b6d5 h5h4 g3h3 c2d1 d5e7 g6h7 h3g2 d1c2 e3f2 "
            "c2c4 f2d2 f6e4 f3e4 c4e4 g2f2 e4g4 d2e3 f7f6 e3f3 g4d4 f2g2 d4b2 g2h3 h7g7 e7f5 g7g6 "
            "f3d3 e5e4 d3e4 b2b3 f5e3 g6f7 e4h7 f7f8 h7h6 f8e8 h6g6 e8d7 g6g7 d7e8 g7g6 e8d7 g6g7 "
            "d7e8 g7g6 e8d7 g6g7 d7e8 g7g6 e8d7 g6h7 d7e8 h7e4 e8f7 e4b7 f7e8 b7c8 e8f7 c8b7 f7e8 "
            "b7c8 e8e7 c8c5 e7f7 c5a7 f7f8 a7c5 f8f7 c5a7 f7e8 a7b8 e8f7 b8b7 f7e8 b7a8 e8f7 a8a7 "
            "f7e8 a7a8 e8d7 a8b7 d7e8 b7c6 e8e7 c6f3 e7f7 f3h5 f7e7 h5f3 e7f7 f3h5 f7e7 h5f3 e7f7 "
            "f3b7 f7e8 b7e4 e8f7 e4h7 f7e8 h7h8 e8e7 h8g7 e7e8 g7g6 e8e7 g6g7 e7e8 g7h8 e8d7 h8g7 "
            "d7e8 g7h8 e8d7 h8g7 d7e8 g7h8 e8d7 h8g7 d7e8 g7g6 e8d7 g6g7 d7e8 g7g6 e8d7 g6e4 b3a3 "
            "e4b7 d7e6 b7d5 e6e7 d5e4 e7d7 e4b7 d7e6 b7d5 e6e7 d5b7 e7e6 b7d5 e6e7 d5b7 e7e6 b7d5 "
            "e6e7 d5b7 e7e6 b7d5 e6e7 d5b7 e7f8 b7a8 f8f7 a8d5 f7f8 d5d8 f8f7 d8d5 f7e8 d5a8 e8d7 "
            "a8b7 d7e6 b7d5 e6e7 d5e4 e7d7 e4b7 d7e6 b7d5 e6e7 d5b7 e7e6 b7d5 e6e7 d5e4 e7d7 e4b7 "
            "d7e6 b7d5 e6e7 d5e4 e7d7 e4b7 d7e6 b7e4 e6d7 e4b7 d7e6 b7d5 e6e7 d5e4 e7d7 e4b7 d7e6 "
            "b7d5 e6e7 d5e4 e7d7 e4b7 d7e6 b7d5 e6e7 d5e4 e7d7 e4b7 d7e6 b7d5 e6e7 d5b7 e7e6 b7d5 "
            "e6e7 d5b7 e7f8 b7a8 f8f7 a8d5 f7e7 d5e4 e7d8 h3g4 a3a4 e4a4 b5a4 e3c2 d8e7 g4f5 e7d6 "
            "f5e4 d6c5 e4d3 c5d6 d3e4 d6c5 e4d3 c5d5 c2b4 d5d6 b4a6 a4a3 a6b4 f6f5 d3d2 f5f4 d2d3 "
            "g5g4 d3e2 d6e5 b4a2 e5d4 e2f1 d4e3 f1g1 e3f3 a2c3 g4g3 c3b5 a3a2 b5d4 f3e3 d4b3 g3h2 "
            "g1h2 e3f2 b3a1 f2e2 a1c2 e2d2 c2d4 d2d3 d4b3 d3c3");
        std::string token;
        ss >> token;

        while (!ss.fail()) {
            game.Update(Convert::StrToMove(token, game.Player()));
            ss >> token;
        }

        std::string_view actual = game.Fen();

        Assert::Equal(actual, expected);
    });
}

static void GameManagerFailure()
{
    TEST("GameManager::AllMoves: london - one failure - end", []() {
        std::string expected =
            "r1bqk2r/ppp2ppp/2n1pn2/b2pN3/3P1B2/2P1P3/PP3PPP/RN1QKB1R w KQkq - 1 6";
        GameManager game(Fen::DEFAULT);

        std::vector<std::string> moves{"d2d4", "d7d5", "g1f3", "g8f6", "c1f4", "b8c6", "e2e3",
                                       "e7e6", "f3e5", "f8b4", "c2c3", "b4a5", "a2a5"};
        for (const auto& move : moves) {
            game.Update(Convert::StrToMove(move, game.Player()));
        }

        Assert::Equal<std::string>(game.Fen().data(), expected);
    });

    TEST("GameManager::AllMoves: london - one failure - mid", []() {
        std::string expected =
            "r1bqk2r/ppp2ppp/2n1pn2/b2pN3/3P1B2/2P1P3/PP3PPP/RN1QKB1R w KQkq - 1 6";
        GameManager game(Fen::DEFAULT);

        std::vector<std::string> moves{"d2d4", "d7d5", "g1f3", "g8f6", "c1f4", "a8a1", "b8c6",
                                       "e2e3", "e7e6", "f3e5", "f8b4", "c2c3", "b4a5"};
        for (const auto& move : moves) {
            game.Update(Convert::StrToMove(move, game.Player()));
        }

        Assert::Equal<std::string>(game.Fen().data(), expected);
    });

    TEST("GameManager::AllMoves: london - three failures - mid", []() {
        std::string expected =
            "r1bqk2r/ppp2ppp/2n1pn2/b2pN3/3P1B2/2P1P3/PP3PPP/RN1QKB1R w KQkq - 1 6";
        GameManager game(Fen::DEFAULT);

        std::vector<std::string> moves{"d2d4", "e2e4", "d7d5", "g1f3", "g8f6",
                                       "e1g1", "c1f4", "b8c6", "e2e3", "e7e6",
                                       "f3e5", "f6d5", "f8b4", "c2c3", "b4a5"};
        for (const auto& move : moves) {
            game.Update(Convert::StrToMove(move, game.Player()));
        }

        Assert::Equal<std::string>(game.Fen().data(), expected);
    });
}

void GameManagerTests()
{
    Promotion();
    Castling();

    GameManagerSuccess();
    GameManagerFailure();
}
