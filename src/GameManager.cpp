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
    m_isWhiteTurn(true), m_isWhiteAI(false), m_isBlackAI(false),
    m_inCheckmate(false), m_inStalemate(false)
{
    fen = m_board.Fen();
    u64 index = fen.find(' ');
    std::string_view player = fen.substr(index + 1);

    if (player[0] == 'b') {
        m_isWhiteTurn = false;
    }

    // Load moves from settings
    std::string defaultNoLastMove = DEFAULT_FEN.data();
    defaultNoLastMove[defaultNoLastMove.size() - 1] = '0';
    if (m_board.Fen() != defaultNoLastMove) {
        std::string moves = Settings::s(Setting::GAME_MOVES);
        u64 start = 0, end = 0;
        while ((end = moves.find(" ", start)) != std::string::npos) {
            m_moves.push_back(moves.substr(start, end - start));
            start = end + 1;
        }
        m_moves.push_back(moves.substr(start));
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

bool GameManager::InCheckmate() const noexcept
{
    return m_moveGen.IsCheckmate();
}

bool GameManager::InStalemate() const noexcept
{
    return m_moveGen.IsStalemate();
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
    return (m_isWhiteTurn ? Enums::Colour::White : Enums::Colour::Black);
}

Index GameManager::Promotion() const noexcept
{
    return m_promotionSquare;
}



// ----- Update -----

void GameManager::Update(std::string_view move)
{
    if (m_inCheckmate || m_inStalemate) {
        return;
    }

    if (move.length() > 0) {
        Update(move, true);
    }
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
        m_currentMove = Convert::CastleToMove(passedMove, Player());
    } else {
        m_currentMove += passedMove;
    }

    // Player attempting to pick up a piece
    if (m_currentMove.length() == 2) {
        // PickupPiece(passedMove);
        // Current move not complete, add it in
        Index index = Convert::MoveToIndex(passedMove);
        if (CheckPieceSelectable(index)) {
            m_moveGen.Generate(m_board.Pieces(), index, m_board.Castling());
            m_possibleMoves = m_moveGen.GetMoves();
        }

        // Failed to generate moves
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
    Index end = Convert::MoveToIndex(m_currentMove.substr(2));

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

    Settings::s(Setting::GAME_FEN, Fen().data());
    Settings::s(Setting::GAME_MOVES, AllMoves());

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
    m_inCheckmate = m_moveGen.IsCheckmate();
    m_inStalemate = m_moveGen.IsStalemate();
    return;
    Enums::Colour attackers = (
        Player() == Enums::Colour::White ? Enums::Colour::Black : Enums::Colour::White
    );

    // Get king pos
    BitBoard kingPos = 0;
    for (Index i = 0; i < 64; i++) {
        const Piece& piece = m_board.Pieces()[i];
        if (piece.Type() == Enums::Type::King && piece.Colour() == Player()) {
            kingPos = Convert::IndexToBitBoard(piece.Position());
            break;
        }
    }

    // Something has gone wrong
    if (kingPos == 0) {
        ErrorPrintln("GameManager::CheckForCheckmate: No {} king found.", Enums::ToString::Colour[(u8)Player()]);
        exit(1);
    }

    // Find a check
    bool inCheck = false;
    for (Index i = 0; i < 64; i++) {
        const Piece& piece = m_board.Pieces()[i];
        if (!piece.IsValid() || piece.Colour() != attackers) {
            continue;
        }

        m_moveGen.Generate(m_board.Pieces(), piece.Position(), 0);
        BitBoard moves = m_moveGen.GetMoves();
        if (moves & kingPos) {
            // Check
            inCheck = true;
            break;
        }
    }

    // In check, check for any valid moves
    Enums::Colour defenders = Player();
    for (Index i = 0; i < 64; i++) {
        const Piece& piece = m_board.Pieces()[i];
        if (!piece.IsValid() || piece.Colour() != defenders) {
            continue;
        }

        m_moveGen.Generate(m_board.Pieces(), piece.Position(), 0);
        BitBoard moves = m_moveGen.GetMoves();
        if (moves != Convert::IndexToBitBoard(piece.Position())) {
            DebugPrintln("GameManager::CheckForCheckmate: Not in checkmate");
            return;
        }
    }

    if (inCheck) {
        DebugPrintln("GameManager::CheckForCheckmate: In checkmate");
        m_inCheckmate = true;
        m_isWhiteTurn = !m_isWhiteTurn;
        Settings::s(Setting::GAME_FEN, DEFAULT_FEN.data());
        Settings::s(Setting::GAME_MOVES, "");
    } else {
        DebugPrintln("GameManager::CheckForCheckmate: In stalemate");
        m_inStalemate = true;
    }
}

