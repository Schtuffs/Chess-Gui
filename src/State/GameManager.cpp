#include "State/GameManager.h"

#include <thread>

#include "Utils/Convert.h"
#include "Utils/Fen.h"
#include "Utils/Settings.h"
#include "Utils/Utils.h"

constexpr const char* DEPTH_COMMAND = "go depth 10";

// ----- Creation / Destruction -----

GameManager::GameManager(std::string_view fen)
    : m_position(fen), m_possibleMoves(0ull), m_selectedSquare(SQ_BAD), m_promotionSquare(SQ_BAD),
      m_isWhiteAI(false), m_isBlackAI(false), m_isReady(false)
{
    m_isWhiteTurn = (m_position.Player() == WHITE);

    // Load moves from settings
    if (m_position.Fen() != Fen::DEFAULT) {
        std::string moves = Settings::s(Setting::GAME_MOVES);
        u64         start = 0, end = 0;
        while ((end = moves.find(" ", start)) != std::string::npos) {
            m_allMoves.push_back(moves.substr(start, end - start));
            start = end + 1;
        }
        m_allMoves.push_back(moves.substr(start));
    }

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
        if (m_isWhiteTurn) {
            std::string position = "position startpos";
            if (!m_allMoves.empty()) {
                position += " moves ";
                position += AllMoves();
            }
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
        if (!m_isWhiteTurn) {
            std::string position = "position startpos";
            if (!m_allMoves.empty()) {
                position += " moves ";
                position += AllMoves();
            }
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

bool GameManager::InCheckmate() const noexcept
{
    return (m_list.size == 0 && m_position.Checkers() != 0);
}

bool GameManager::InStalemate() const noexcept
{
    return (m_list.size == 0 && m_position.Checkers() == 0);
}

std::string_view GameManager::Fen() { return m_position.Fen(); }

BitBoard GameManager::Moves() const noexcept { return m_possibleMoves; }

Colour GameManager::Player() const noexcept { return m_position.Player(); }

Square GameManager::Promotion() const noexcept { return m_promotionSquare; }

// ----- Update -----

bool GameManager::Pickup(Square sq)
{
    // Cant update if game finished
    if (InCheckmate() || InStalemate()) {
        return false;
    }

    // Reset state always
    m_selectedSquare = SQ_BAD;
    m_possibleMoves  = 0ull;

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

bool GameManager::Promote(PieceType type) noexcept
{
    if (!m_promotionMove.IsValid()) {
        WarningPrintln("GameManager::Promote: Invalid promotion move: {}", m_promotionMove.Str());
        return false;
    }

    if (type != QUEEN && type != ROOK && type != BISHOP && type != KNIGHT) {
        WarningPrintln("GameManager::Promote: Invalid promo type: {}", (u8)type);
        return false;
    }

    OnValidMove(Move::MakePromo(m_promotionMove.From(), m_promotionMove.To(), type));
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
    }
    // Pickup and move in 1
    else if (m_selectedSquare == SQ_BAD) {
        Pickup(move.From());
    }

    MakeMove(move);
}

// ----- Hidden -----

void GameManager::EngineUpdate(Pipes::ID id)
{
    constexpr const char SEARCH[] = "bestmove";

    std::string str = Pipes::Read(id, false);
    auto        idx = str.find(SEARCH);
    if (idx == std::string::npos) {
        return;
    }

    idx += sizeof(SEARCH);
    str       = str.substr(idx);
    u64 space = str.find(' ');
    if (space == std::string::npos) {
        space = 4;
    }

    std::string strMove = str.substr(0, space);
    Move        move    = Convert::StrToMove(strMove, Player());
    if (move.IsValid()) {
        std::println("{} Move: {}", strMove, move.Str());
        Pickup(move.From());
        MakeMove(move);
    }
}

void GameManager::MakeMove(Move move)
{
    // Must be valid move
    if (!move.IsValid()) {
        ErrorPrintln("GameManager::MakeMove: Invalid move: {}", move.Str());
        return;
    }

    // Manage the promotion taking place
    if (Utils::IsValidSquare(m_promotionSquare)) {
        return;
    }

    // Translate special moves
    if (m_position.Pieces(Player(), KING) & move.From()) {
        if (std::abs((i8)move.To() - (i8)move.From()) == 2) {
            move = Move::MakeCastle(move.From(), move.To());
        }
    }

    // Special pawn
    if (m_position.Pieces(Player(), PAWN) & move.From()) {
        if (std::abs((i8)move.To() - (i8)move.From()) == 16) {
            move = Move::MakeEnPassant(move.From(), move.To());
        }
    }

    // Try to play the move
    if (CheckMove(move)) {
        if (!CheckPromotion(move)) {
            return;
        }

        OnValidMove(move);
        return;
    }

    // Reselection
    if (move.To() != m_selectedSquare) {
        Pickup(move.To());
    } else {
        m_selectedSquare = SQ_BAD;
        m_possibleMoves  = 0ull;
    }
}

bool GameManager::CheckMove(Move move)
{
    if (m_selectedSquare != SQ_BAD && move.From() != m_selectedSquare) {
        WarningPrintln("GameManager::CheckMove: Invalid start square: {}", move.Str());
        return false;
    }

    if (m_selectedSquare != SQ_BAD && !(m_possibleMoves & move.To())) {
        WarningPrintln("GameManager::CheckMove: Invalid target square: {}", move.Str());
        return false;
    }

    if (!m_position.IsLegal(move)) {
        WarningPrintln("GameManager::CheckMove: Illegal move: {}", move.Str());
        return false;
    }

    if (!(m_possibleMoves & move.To())) {
        WarningPrintln("GameManager::CheckMove: Illegal move to: {}", move.Str());
        return false;
    }

    return true;
}

bool GameManager::CheckPromotion(Move move)
{
    if (m_position.Pieces(PAWN) & move.From() && !move.IsPromo()) {
        BitBoard rank8 = (Player() == WHITE ? RANK_8BB : RANK_1BB);
        if (rank8 & move.To()) {
            m_possibleMoves   = 0ull;
            m_promotionSquare = move.To();
            m_promotionMove   = move;
            return false;
        }
        return true;
    }

    if (!move.IsPromo()) {
        return true;
    }

    if (!(m_position.Pieces(PAWN) & move.From())) {
        return false;
    }

    BitBoard rank8 = (Player() == WHITE ? RANK_8BB : RANK_1BB);
    if (!(rank8 & move.To())) {
        return false;
    }

    m_possibleMoves   = 0ull;
    m_promotionSquare = move.To();
    m_promotionMove   = move;
    return true;
}

bool GameManager::CheckPieceSelectable(Square sq)
{
    return (m_position.Pieces(Player()) & sq).raw();
}

void GameManager::OnValidMove(Move move)
{
    // Set data
    m_selectedSquare  = SQ_BAD;
    m_promotionSquare = SQ_BAD;
    m_possibleMoves   = 0ull;
    m_isWhiteTurn     = !m_isWhiteTurn;
    m_allMoves.push_back(Convert::MoveToStr(move));
    m_position.MakeMove(move);
    DebugPrintln("GameManager::OnValidMove: New fen: {}", Fen());

    m_list.Clear();
    MoveGen::Generate(m_position, m_list);
    m_list.Legalize(m_position);

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
