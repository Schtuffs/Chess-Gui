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
    m_promotionSquare(64),
    m_isWhiteTurn(true), m_isWhiteAI(false), m_isBlackAI(false)
{
    fen = m_board.Fen();
    u64 index = fen.find(' ');
    std::string_view player = fen.substr(index + 1);

    if (player[0] == 'b') {
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

Enums::Colour GameManager::Player() const noexcept
{
    return m_board.Player();
}

Index GameManager::Promotion() const noexcept
{
    return m_promotionSquare;
}



// ----- Update -----

void GameManager::Update(std::string_view move)
{
    if (move.length() > 0) {
        Update(move, true);
    }
}

// ----- Hidden -----

void GameManager::Update(std::string_view passedMove, bool tryReselect)
{
    static std::string move;

    // Move requires file and rank
    if (passedMove.length() < 2) {
        WarningPrintln("GameManager::Update: passed move too small: \"{}\"", passedMove);
        return;
    }

    // Manage the promotion taking place
    if (Utils::IsValidIndex(m_promotionSquare)) {
        move.clear();
        ManagePromotion(passedMove);
        return;
    }

    // Prepare state information
    bool isSameIndex = (move == passedMove);
    if (passedMove.length() >= 4) {
        move = passedMove;
    } else {
        move += passedMove;
    }

    if (move.length() == 2) {
        // Current move not complete, add it in
        Index index = Convert::MoveToIndex(passedMove);
        if (CheckPieceSelectable(index)) {
            m_possibleMoves = m_moveGen.Generate(m_board.Pieces().data(), index, m_board.Castling(m_board.Player()));
        }

        // Failed to generate moves
        if (m_possibleMoves == MoveGen::INVALID) {
            move.clear();
        }

        return;
    }

    // Try to play the move
    bool moveCheck = CheckMove(move);
    if (moveCheck) {
        OnValidMove(move);
    }

    // Clear old data
    move.clear();
    m_possibleMoves = MoveGen::INVALID;

    // Try to reselect
    if (!moveCheck && tryReselect && !isSameIndex) {
        Update(passedMove, false);
    }
}

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

void GameManager::OnValidMove(std::string_view move)
{
    m_isWhiteTurn = !m_isWhiteTurn;
    m_moves.push_back(move.data());
    std::string saveFen = Fen().data();
    saveFen += " " + AllMoves();
    Settings::s(Setting::GAME_FEN, saveFen);

    CheckForPromotion(move);
    CheckForCheckmate();
}

void GameManager::CheckForPromotion(std::string_view move)
{
    move = move.substr(2);
    Index index = Convert::MoveToIndex(move);
    const Piece& piece = m_board.Pieces()[index];

    if (piece.Type() != Enums::Type::Pawn) {
        return;
    }

    if ((index / 8) == 0 && piece.Colour() == Enums::Colour::Black) {
        m_promotionSquare = index;
    }
    else if ((index / 8) == 7 && piece.Colour() == Enums::Colour::White) {
        m_promotionSquare = index;
    }
    else {
        m_promotionSquare = 64;
    }

    ManagePromotion(move.substr(2));
}

void GameManager::ManagePromotion(std::string_view move)
{
    constexpr u8 TOTAL_PROMOTIONS                       = 4;
    constexpr Enums::Type PROMOTIONS[TOTAL_PROMOTIONS]  = {Enums::Type::Queen, Enums::Type::Rook, Enums::Type::Bishop, Enums::Type::Knight};
    constexpr const char* PROMOTIONS_CHAR               = "qrbn";

    if (move.length() == 0) {
        return;
    }

    if (move.length() % 2 == 1) {
        // The promotion char
        char promotion = move[move.length() - 1];

        // Determine the type
        size_t i;
        for (i = 0; i < sizeof(PROMOTIONS_CHAR); i++) {
            if (promotion == PROMOTIONS_CHAR[i]) {
                break;
            }
        }

        if (i == sizeof(PROMOTIONS_CHAR)) {
            WarningPrintln("GameManager::ManagePromotion: Invalid promotion type: {}", promotion);
            return;
        }

        if (!m_board.PromotePawn(m_promotionSquare, PROMOTIONS[i])) {
            WarningPrintln("GameManager::ManagePromotion: Could not promote pawn.");
            return;
        }

        m_moves[m_moves.size() - 1] += PROMOTIONS_CHAR[i];
        m_promotionSquare = 64;

        return;
    }

    Index clicked = Convert::MoveToIndex(move);
    i8 sign = (m_promotionSquare / 8 == 0 ? 1 : -1);

    for (u8 i = 0; i < TOTAL_PROMOTIONS; i++) {
        Index index = m_promotionSquare + (sign * (i8)(i * 8));
        if (clicked == index) {
            if (m_board.PromotePawn(m_promotionSquare, PROMOTIONS[i])) {
                m_moves[m_moves.size() - 1] += PROMOTIONS_CHAR[i];
                m_promotionSquare = 64;
            }
            else {
                WarningPrintln("GameManager::ManagePromotion: Could not promote pawn.");
            }
            return;
        }
    }
}

void GameManager::CheckForCheckmate()
{

}

