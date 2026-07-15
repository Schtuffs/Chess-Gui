#include "GameManager.h"

#include "Convert.h"
#include "MoveGen.h"
#include "Settings.h"
#include "Utils.h"

// ----- Creation / Destruction -----

GameManager::GameManager(std::string_view fen)
  : m_board(fen),
    m_moveGen(),
    m_possibleMoves(0),
    m_isWhiteTurn(true), m_isWhiteAI(false), m_isBlackAI(false)
{
    fen = m_board.Fen();
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
        OnButtonPress(move, true);
    }
}

// ----- Hidden -----

bool GameManager::CheckMove(std::string& move)
{
    return (m_board.MakeMove(move));
}

bool GameManager::CheckPieceSelectable(Index index)
{
    Enums::Colour col = m_board.Pieces()[index].Colour();
    return (
        (m_isWhiteTurn && col == Enums::Colour::White) ||
        (!m_isWhiteTurn && col == Enums::Colour::Black)
    );
}

void GameManager::OnButtonPress(std::string_view passedMove, bool tryReselect)
{
    static std::string move;
    bool isSameIndex = (move == passedMove);
    move += passedMove;

    if (m_possibleMoves == MoveGen::INVALID) {
        Index index = Convert::MoveToIndex(passedMove);
        if (CheckPieceSelectable(index)) {
            m_possibleMoves = m_moveGen.Generate(m_board.Pieces(), index, m_board.Castling(m_board.Player()));
        }

        if (m_possibleMoves == MoveGen::INVALID) {
            move.clear();
        }
    }
    else {
        bool moveCheck = CheckMove(move);
        if (moveCheck) {
            m_isWhiteTurn = !m_isWhiteTurn;
            m_moves.push_back(move);
            Settings::s(Setting::GAME_FEN, Fen());
        }

        move.clear();
        m_possibleMoves = MoveGen::INVALID;
        
        if (!moveCheck && tryReselect && !isSameIndex) {
            OnButtonPress(passedMove, false);
        }
    }
}
