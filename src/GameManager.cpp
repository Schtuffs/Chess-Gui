#include "GameManager.h"

#include "Convert.h"
#include "MoveGen.h"
#include "Settings.h"
#include "Utils.h"

// ----- Creation / Destruction -----

GameManager::GameManager()
    : m_board(Settings::s(Setting::GAME_STATE)), m_isWhiteTurn(true)
{
    std::string_view fen = m_board.Fen();
    u64 index = fen.find(' ');
    fen = fen.substr(index + 1);

    if (fen[0] == 'b') {
        m_isWhiteTurn = false;
    }
}

GameManager::~GameManager()
{}



// ----- Read -----

std::string GameManager::AllMoves() const noexcept
{
    std::string moves;
    if (m_moves.size() == 0) {
        return moves;
    }

    moves += m_moves[0];
    for (size_t i = 1; i < m_moves.size(); i++) {
        moves += ' ';
        moves += m_moves[i];
    }

    return moves;
}

std::string_view GameManager::Fen() const noexcept
{
    return m_board.Fen();
}

BitBoard GameManager::Moves() const noexcept
{
    return m_possibleMoves;
}



// ----- Update -----

void GameManager::Update(std::string_view move)
{
    if (move.length() > 0) {
        OnButtonPress(move);
    }
}

// ----- Hidden -----

void GameManager::CheckMove(std::string& move)
{
    if (m_board.MakeMove(move)) {
        m_isWhiteTurn = !m_isWhiteTurn;
        m_moves.push_back(move);
    }
    
    move.clear();
    m_possibleMoves = 0;
}

void GameManager::OnButtonPress(std::string_view passedMove)
{
    static std::string move;
    Index index = Convert::MoveToIndex(passedMove);
    move += passedMove;
    if (m_possibleMoves == 0) {
        Enums::Colour col = m_board.Pieces()[index].Colour();
        if (
            (m_isWhiteTurn && col == Enums::Colour::Black) ||
            (!m_isWhiteTurn && col == Enums::Colour::White)
        ) {
            move.clear();
            return;
        }
        
        m_possibleMoves = MoveGen::Generate(m_board, m_board.Pieces()[index]);
        if (m_possibleMoves == 0) {
            move.clear();
        }
    }
    else {
        CheckMove(move);
    }
}
