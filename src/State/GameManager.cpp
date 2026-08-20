#include "State/GameManager.h"

#include <thread>

#include "Utils/Convert.h"
#include "Utils/Settings.h"
#include "Utils/Utils.h"

constexpr const char* DEPTH_COMMAND = "go depth 10";

// ----- Creation / Destruction -----

GameManager::GameManager(std::string_view fen)
    : m_position(fen), m_possibleMoves(0), m_selectedSquare(SQ_BAD), m_promotionSquare(SQ_BAD),
      m_isWhiteTurn(true), m_isWhiteAI(false), m_isBlackAI(false), m_isReady(false)
{
    // fen                     = m_position.Fen();
    u64              sq     = fen.find(' ');
    std::string_view player = fen.substr(sq + 1);

    if (player[0] == 'b') {
        m_isWhiteTurn = false;
    }

    // Load moves from settings
    std::string moves = Settings::s(Setting::GAME_MOVES);
    u64         start = 0, end = 0;
    while ((end = moves.find(" ", start)) != std::string::npos) {
        m_allMoves.push_back(moves.substr(start, end - start));
        start = end + 1;
    }
    m_allMoves.push_back(moves.substr(start));

    // Setup engines
    m_isWhiteAI = Settings::b(Setting::ENGINE_WHITE_AI);
    m_isBlackAI = Settings::b(Setting::ENGINE_BLACK_AI);
    if (!m_isWhiteAI && !m_isBlackAI) {
        m_isReady = true;
    }

    // Generate all legal moves
    MoveGen::Generate(m_position, m_list);
    m_list.Legalize(m_position);
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
        if (!m_allMoves.empty()) {
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
        if (!m_allMoves.empty()) {
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
    if (m_allMoves.size() == 0) {
        return moves;
    }

    moves += m_allMoves[0];
    for (size_t i = 1; i < m_allMoves.size(); i++) {
        moves += ' ';
        moves += m_allMoves[i];
    }

    return moves;
}

Square GameManager::Held() const noexcept { return m_selectedSquare; }

bool GameManager::InCheckmate() const noexcept { return false; }

bool GameManager::InStalemate() const noexcept { return false; }

std::string_view GameManager::Fen() { return m_position.Fen(); }

BitBoard GameManager::Moves() const noexcept { return m_possibleMoves; }

Colour GameManager::Player() const noexcept { return m_position.Player(); }

Square GameManager::Promotion() const noexcept { return m_promotionSquare; }

// ----- Update -----

bool GameManager::Pickup(Square sq)
{
    // Reset state always
    m_selectedSquare = SQ_BAD;
    m_possibleMoves  = 0;

    // Pickup piece of current player
    if (!(m_position.Pieces(Player()) & sq)) {
        return false;
    }

    // Select square and get the legal moves
    m_selectedSquare = sq;
    m_possibleMoves |= sq;
    m_possibleMoves |= m_list.ToBB(m_selectedSquare);

    return true;
}

void GameManager::Update() { Update(Move(0)); }

void GameManager::Update(Move move)
{
    // Cant update if game finished
    if (InCheckmate() || InStalemate()) {
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

    // Must be valid move
    if (!move.IsValid()) {
        return;
    }

    // Check for piece selection being required
    if (m_selectedSquare != SQ_BAD && move.From() != m_selectedSquare) {
        Pickup(move.From());
        return;
    }

    MakeMove(move);
}

// ----- Hidden -----

void GameManager::EngineUpdate(Pipes::ID id)
{
    constexpr const char SEARCH[] = "bestmove";

    std::string str = Pipes::Read(id, false);
    auto        sq  = str.find(SEARCH);
    if (sq == std::string::npos) {
        return;
    }

    sq += sizeof(SEARCH);
    str       = str.substr(sq);
    u64 space = str.find(' ');
    if (space == std::string::npos) {
        space = 4;
    }

    str = str.substr(0, space);
    if (str.length() >= 4) {
        Update(Convert::StrToMove(str));
    }
}

void GameManager::MakeMove(Move move)
{
    if (!move.IsValid()) {
        ErrorPrintln("GameManager::MakeMove: Invalid move: {}", move.Str());
        return;
    }

    // Manage the promotion taking place
    if (Utils::IsValidSquare(m_promotionSquare)) {
        ManagePromotion(move);
        return;
    }

    // Try to play the move
    if (CheckMove(move)) {
        OnValidMove(move);
        return;
    }

    // Reselection
    if (move.To() != m_selectedSquare) {
        Pickup(move.To());
    } else {
        m_selectedSquare = SQ_BAD;
        m_possibleMoves  = 0;
    }
}

bool GameManager::CheckMove(Move move) { return m_position.IsLegal(move); }

bool GameManager::CheckPieceSelectable(Square sq)
{
    return (m_position.Pieces(Player()) & sq).raw();
}

void GameManager::OnValidMove(Move move)
{
    // Set data
    m_selectedSquare = SQ_BAD;
    m_possibleMoves  = 0;
    m_isWhiteTurn    = !m_isWhiteTurn;
    m_allMoves.push_back(Convert::MoveToStr(move));

    // Manage the promotion
    if (move.IsPromo()) {
        ManagePromotion(move);
    }

    // Save data
    Settings::s(Setting::GAME_FEN, Fen().data());
    Settings::s(Setting::GAME_MOVES, AllMoves());

    // Setup engine for next move
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
}

void GameManager::ManagePromotion(Move move)
{
    // Must be promo
    if (!move.IsPromo()) {
        return;
    }

    // m_promotionSquare                      = SQ_BAD;
    // constexpr u8          TOTAL_PROMOTIONS = 4;
    // constexpr const char* PROMOTIONS_CHAR  = "qrbn";

    // if (move.length() == 0) {
    //     return;
    // }

    // if (!Utils::IsValidSquare(m_promotionSquare)) {
    //     return;
    // }

    // if (move.length() % 2 == 1) {
    //     // The promotion char
    //     char promotion = move[move.length() - 1];

    //     // Determine the type
    //     size_t i;
    //     for (i = 0; i < TOTAL_PROMOTIONS; i++) {
    //         if (promotion == PROMOTIONS_CHAR[i]) {
    //             break;
    //         }
    //     }

    //     if (i == TOTAL_PROMOTIONS) {
    //         WarningPrintln("GameManager::ManagePromotion: Invalid promotion type: {}",
    //         promotion); return;
    //     }

    //     if (!m_position.IsLegal(move)) {
    //         WarningPrintln("GameManager::ManagePromotion: Board could not promote: {}", move);
    //         return;
    //     }

    //     m_promotionSquare = 64;
    //     OnValidMove(move);

    //     return;
    // }

    // Square clicked = Convert::MoveToIndex(move);
    // i8     sign    = (m_promotionSquare / 8 == 0 ? 1 : -1);

    // for (u8 i = 0; i < TOTAL_PROMOTIONS; i++) {
    //     Square sq = m_promotionSquare + (sign * (i8)(i * 8));
    //     if (clicked == sq) {
    //         std::string promo = m_currentMove + PROMOTIONS_CHAR[i];
    //         if (m_board.MakeMove(promo)) {
    //             m_promotionSquare = 64;
    //             OnValidMove(promo);
    //         } else {
    //             WarningPrintln("GameManager::ManagePromotion: Could not promote pawn.");
    //         }
    //         return;
    //     }
    // }
}
