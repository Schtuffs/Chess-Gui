#include "GameManager.h"

#include "Convert.h"
#include "Settings.h"
#include "Utils.h"

// ----- Creation / Destruction -----

GameManager::GameManager(std::string_view fen)
    : m_board(fen), m_moveGen(), m_possibleMoves(0), m_promotionSquare(64), m_isWhiteTurn(true),
      m_isWhiteAI(false), m_isBlackAI(false)
{
    fen                     = m_board.Fen();
    u64              index  = fen.find(' ');
    std::string_view player = fen.substr(index + 1);

    if (player[0] == 'b') {
        m_isWhiteTurn = false;
    }

    // Load moves from settings
    std::string defaultNoLastMove                   = DEFAULT_FEN.data();
    defaultNoLastMove[defaultNoLastMove.size() - 1] = '0';
    if (m_board.Fen() != defaultNoLastMove) {
        std::string moves = Settings::s(Setting::GAME_MOVES);
        u64         start = 0, end = 0;
        while ((end = moves.find(" ", start)) != std::string::npos) {
            m_moves.push_back(moves.substr(start, end - start));
            start = end + 1;
        }
        m_moves.push_back(moves.substr(start));
    }

    m_moveGen.Generate(m_board, Player());
    m_isWhiteAI = Settings::b(Setting::ENGINE_WHITE_AI);
    m_isBlackAI = Settings::b(Setting::ENGINE_BLACK_AI);

    // Detect promo
    for (int i = 0; i < 16; i++) {
        if ((i / 8) == 1) {
            // White promo
            Index index = (i % 8) + 56;
            if (m_board.Pieces()[index].Type() == Enums::Type::Pawn) {
                m_promotionSquare = index;
                break;
            }
        } else {
            // Black promo
            Index index = (i % 8);
            if (m_board.Pieces()[index].Type() == Enums::Type::Pawn) {
                m_promotionSquare = index;
                break;
            }
        }
    }
}

GameManager::~GameManager() {}

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

bool GameManager::InCheckmate() const noexcept { return m_moveGen.IsCheckmate(); }

bool GameManager::InStalemate() const noexcept { return m_moveGen.IsStalemate(); }

std::string_view GameManager::Fen() { return m_board.Fen(); }

BitBoard GameManager::Moves() const noexcept { return m_possibleMoves; }

Enums::Colour GameManager::Player() const noexcept
{
    return (m_isWhiteTurn ? Enums::Colour::White : Enums::Colour::Black);
}

Index GameManager::Promotion() const noexcept { return m_promotionSquare; }

// ----- Update -----

void GameManager::Update(std::string_view move)
{
    if (m_moveGen.IsCheckmate() || m_moveGen.IsStalemate()) {
        return;
    }

    if (move.length() < 2) {
        return;
    }

    move = Convert::CastleToMove(move, Player());
    if (!Utils::IsValidIndex(Convert::MoveToIndex(move))) {
        return;
    }

    Update(move, true);
}

// ----- Hidden -----

void GameManager::Update(std::string_view passedMove, bool tryReselect)
{
    // Move requires file and rank
    if (passedMove.length() < 2) {
        WarningPrintln("GameManager::Update: passed move too small: \"{}\"", passedMove);
        return;
    }

    // Manage the promotion taking place
    if (Utils::IsValidIndex(m_promotionSquare)) {
        ManagePromotion(passedMove);
        return;
    }

    // Prepare state information
    if (passedMove.length() >= 3) {
        m_currentMove = passedMove;
    } else {
        m_currentMove += passedMove;
    }

    // Player attempting to pick up a piece
    if (m_currentMove.length() == 2) {
        // Current move not complete, add it in
        Index index = Convert::MoveToIndex(passedMove);
        if (CheckPieceSelectable(index)) {
            m_possibleMoves = m_moveGen.GetMoves(index);
        }

        // Failed to get moves
        if (m_possibleMoves == MoveGen::INVALID) {
            m_currentMove.clear();
        }

        return;
    }

    // Try to play the move
    bool moveCheck = CheckMove(m_currentMove);
    if (moveCheck) {
        OnValidMove(m_currentMove);
    }

    // Prepare indexes for reselection
    Index start = Convert::MoveToIndex(m_currentMove);
    Index end   = Convert::MoveToIndex(m_currentMove.substr(2));

    // Clear old data
    m_currentMove.clear();
    m_possibleMoves = MoveGen::INVALID;

    // Try to reselect
    bool isSameIndex = (start == end);
    if (!moveCheck && tryReselect && !isSameIndex) {
        Update(passedMove, false);
    }
}

bool GameManager::CheckMove(std::string& move)
{
    Index    start  = Convert::MoveToIndex(move);
    Index    endIdx = Convert::MoveToIndex(move.substr(2));
    BitBoard end    = Convert::IndexToBitBoard(endIdx);

    BitBoard moves = m_moveGen.GetMoves(start);
    if ((moves & end) == 0) {
        return false;
    }

    return (m_board.MakeMove(move));
}

bool GameManager::CheckPieceSelectable(Index index)
{
    Enums::Colour col = m_board.Pieces()[index].Colour();
    return ((m_isWhiteTurn && col == Enums::Colour::White) ||
            (!m_isWhiteTurn && col == Enums::Colour::Black));
}

void GameManager::OnValidMove(std::string_view move)
{
    m_isWhiteTurn = !m_isWhiteTurn;
    m_moves.push_back(move.data());

    Settings::s(Setting::GAME_FEN, Fen().data());
    Settings::s(Setting::GAME_MOVES, AllMoves());

    CheckForPromotion(move);

    m_moveGen.Generate(m_board, Player());
}

void GameManager::CheckForPromotion(std::string_view move)
{
    move               = move.substr(2);
    Index        index = Convert::MoveToIndex(move);
    const Piece& piece = m_board.Pieces()[index];

    if (piece.Type() != Enums::Type::Pawn) {
        return;
    }

    if ((index / 8) == 0 && piece.Colour() == Enums::Colour::Black) {
        m_promotionSquare = index;
    } else if ((index / 8) == 7 && piece.Colour() == Enums::Colour::White) {
        m_promotionSquare = index;
    } else {
        m_promotionSquare = 64;
    }

    ManagePromotion(move.substr(2));
}

void GameManager::ManagePromotion(std::string_view move)
{
    constexpr u8          TOTAL_PROMOTIONS             = 4;
    constexpr Enums::Type PROMOTIONS[TOTAL_PROMOTIONS] = {Enums::Type::Queen, Enums::Type::Rook,
                                                          Enums::Type::Bishop, Enums::Type::Knight};
    constexpr const char* PROMOTIONS_CHAR              = "qrbn";

    if (move.length() == 0) {
        return;
    }

    if (!Utils::IsValidIndex(m_promotionSquare)) {
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

        if (m_moves[m_moves.size() - 1].length() == 4) {
            m_moves[m_moves.size() - 1] += PROMOTIONS_CHAR[i];
        }
        m_promotionSquare = 64;
        m_isWhiteTurn = !m_isWhiteTurn;
        m_moveGen.Generate(m_board, Player());
        
        return;
    }
    
    Index clicked = Convert::MoveToIndex(move);
    i8    sign    = (m_promotionSquare / 8 == 0 ? 1 : -1);
    
    for (u8 i = 0; i < TOTAL_PROMOTIONS; i++) {
        Index index = m_promotionSquare + (sign * (i8)(i * 8));
        if (clicked == index) {
            if (m_board.PromotePawn(m_promotionSquare, PROMOTIONS[i])) {
                m_moves[m_moves.size() - 1] += PROMOTIONS_CHAR[i];
                m_promotionSquare = 64;
                m_isWhiteTurn = !m_isWhiteTurn;
                m_moveGen.Generate(m_board, Player());
            } else {
                WarningPrintln("GameManager::ManagePromotion: Could not promote pawn.");
            }
            return;
        }
    }
}
