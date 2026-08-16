#include "GameManager.h"

#include <thread>

#include "Convert.h"
#include "Settings.h"
#include "Utils.h"

constexpr const char* DEPTH_COMMAND = "go depth 10";

// ----- Creation / Destruction -----

GameManager::GameManager(std::string_view fen)
    : m_board(fen), m_moveGen(), m_possibleMoves(0), m_promotionSquare(64), m_isWhiteTurn(true),
      m_isWhiteAI(false), m_isBlackAI(false), m_isReady(false)
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
    if (!m_isWhiteAI && !m_isBlackAI) {
        m_isReady = true;
    }

    // Detect promo
    for (int i = 0; i < 16; i++) {
        if ((i / 8) == 1) {
            // White promo
            Index index = (i % 8) + 56;
            if (m_board.Pieces()[index].Type() == Enums::Type::Pawn) {
                m_promotionSquare = index;
                m_isWhiteTurn     = !m_isWhiteTurn;
                break;
            }
        } else {
            // Black promo
            Index index = (i % 8);
            if (m_board.Pieces()[index].Type() == Enums::Type::Pawn) {
                m_promotionSquare = index;
                m_isWhiteTurn     = !m_isWhiteTurn;
                break;
            }
        }
    }
}

GameManager::~GameManager() {}

static void EngineStart(Pipes::ID& id, const std::string& path)
{
    id = Pipes::Start(path);
    if (!Pipes::IsValid(id)) {
        ErrorPrintln("GameManager::EngineStart: Invalid Pipe created for path: {}", path);
        return;
    }

    DebugPrintln("{}", Pipes::Read(id, true));
    Pipes::Write(id, "uci");
    DebugPrintln("{}", Pipes::Read(id, true));
    Pipes::Write(id, "isready");
    DebugPrintln("{}", Pipes::Read(id, true));
}

void GameManager::IsReady()
{
    // Already ready
    if (m_isReady) {
        return;
    }
    m_isReady = true;

    // Get ready
    if (m_isWhiteAI) {
        std::thread wEngine(EngineStart, std::ref(m_whiteID),
                            Settings::s(Setting::ENGINE_WHITE_PATH));
        wEngine.join();
        std::string position = "position startpos";
        if (!m_moves.empty()) {
            position += " moves ";
            position += AllMoves();
        }
        if (m_isWhiteTurn) {
            if (!Pipes::Write(m_whiteID, position)) {
                ErrorPrintln("GameManager::IsReady: Failed to write position to pipe.");
            }
            if (!Pipes::Write(m_whiteID, DEPTH_COMMAND)) {
                ErrorPrintln("GameManager::IsReady: Failed to write search command to pipe.");
            }
        }
    }
    if (m_isBlackAI) {
        std::thread bEngine(EngineStart, std::ref(m_blackID),
                            Settings::s(Setting::ENGINE_BLACK_PATH));
        bEngine.join();
        std::string position = "position startpos";
        if (!m_moves.empty()) {
            position += " moves ";
            position += AllMoves();
        }
        if (!m_isWhiteTurn) {
            if (!Pipes::Write(m_blackID, position)) {
                ErrorPrintln("GameManager::IsReady: Failed to write position to pipe.");
            }
            if (!Pipes::Write(m_blackID, DEPTH_COMMAND)) {
                ErrorPrintln("GameManager::IsReady: Failed to write search command to pipe.");
            }
        }
    }
}

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

    // Check for engine ops
    if (m_isWhiteTurn && m_isWhiteAI) {
        EngineUpdate(m_whiteID);
        return;
    }
    if (!m_isWhiteTurn && m_isBlackAI) {
        EngineUpdate(m_blackID);
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

void GameManager::EngineUpdate(Pipes::ID id)
{
    constexpr const char SEARCH[] = "bestmove";
    std::string          str      = Pipes::Read(id, false);
    u64                  index    = str.find(SEARCH);
    if (index == std::string::npos) {
        return;
    }

    index += sizeof(SEARCH);
    str       = str.substr(index);
    u64 space = str.find(' ');
    if (space == std::string::npos) {
        space = 4;
    }

    str = str.substr(0, space);
    if (str.length() >= 4) {
        Update(str, false);
    }
}

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
        m_currentMove = Convert::CastleToMove(passedMove, Player());
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
    bool validMove = CheckMove(m_currentMove);
    if (validMove) {
        OnValidMove(m_currentMove);
    } else {
        // If we have a promotion, no need for reselection
        CheckForPromotion(m_currentMove);
        if (Utils::IsValidIndex(m_promotionSquare)) {
            return;
        }
    }

    // Prepare indexes for reselection
    Index start = Convert::MoveToIndex(m_currentMove);
    Index end   = Convert::MoveToIndex(m_currentMove.substr(2));

    // Clear old data
    m_currentMove.clear();
    m_possibleMoves = MoveGen::INVALID;

    // Try to reselect
    bool isSameIndex = (start == end);
    if (!validMove && tryReselect && !isSameIndex) {
        Update(passedMove, false);
    }
}

bool GameManager::CheckMove(std::string_view move)
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

    if (m_isWhiteTurn) {
        if (m_isWhiteAI) {
            std::string command = "position startpos moves " + AllMoves();
            if (!Pipes::Write(m_whiteID, command)) {
                ErrorPrintln("GameManager::OnValidMove: Failed to write to white engine.");
            }
            if (!Pipes::Write(m_whiteID, DEPTH_COMMAND)) {
                ErrorPrintln("GameManager::OnValidMove: Failed to write to white engine.");
            }
        }
    } else {
        if (m_isBlackAI) {
            std::string command = "position startpos moves " + AllMoves();
            if (!Pipes::Write(m_blackID, command)) {
                ErrorPrintln("GameManager::OnValidMove: Failed to write to black engine.");
            }
            if (!Pipes::Write(m_blackID, DEPTH_COMMAND)) {
                ErrorPrintln("GameManager::OnValidMove: Failed to write to black engine.");
            }
        }
    }

    m_moveGen.Generate(m_board, Player());
}

void GameManager::CheckForPromotion(std::string_view move)
{
    Index        start = Convert::MoveToIndex(move);
    Index        end   = Convert::MoveToIndex(move.substr(2));
    const Piece& piece = m_board.Pieces()[start];

    if (piece.Type() != Enums::Type::Pawn) {
        return;
    }

    if ((end / 8) == 0 && piece.Colour() == Enums::Colour::Black) {
        m_promotionSquare = end;
        m_possibleMoves   = MoveGen::INVALID;
    } else if ((end / 8) == 7 && piece.Colour() == Enums::Colour::White) {
        m_promotionSquare = end;
        m_possibleMoves   = MoveGen::INVALID;
    } else {
        m_promotionSquare = 64;
    }

    // Only if it contains promotion details
    if (move.length() % 2 == 1) {
        ManagePromotion(move.substr(2));
    }
}

void GameManager::ManagePromotion(std::string_view move)
{
    constexpr u8          TOTAL_PROMOTIONS = 4;
    constexpr const char* PROMOTIONS_CHAR  = "qrbn";

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
        for (i = 0; i < TOTAL_PROMOTIONS; i++) {
            if (promotion == PROMOTIONS_CHAR[i]) {
                break;
            }
        }

        if (i == TOTAL_PROMOTIONS) {
            WarningPrintln("GameManager::ManagePromotion: Invalid promotion type: {}", promotion);
            return;
        }

        if (!m_board.MakeMove(move)) {
            WarningPrintln("GameManager::ManagePromotion: Board could not promote: {}", move);
            return;
        }

        m_promotionSquare = 64;
        OnValidMove(move);

        return;
    }

    Index clicked = Convert::MoveToIndex(move);
    i8    sign    = (m_promotionSquare / 8 == 0 ? 1 : -1);

    for (u8 i = 0; i < TOTAL_PROMOTIONS; i++) {
        Index index = m_promotionSquare + (sign * (i8)(i * 8));
        if (clicked == index) {
            std::string promo = m_currentMove + PROMOTIONS_CHAR[i];
            if (m_board.MakeMove(promo)) {
                m_promotionSquare = 64;
                OnValidMove(promo);
            } else {
                WarningPrintln("GameManager::ManagePromotion: Could not promote pawn.");
            }
            return;
        }
    }
}
