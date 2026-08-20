#include "Utils/Renderer.h"

#include <cctype>
#include <cstring>

#include "Utils/Convert.h"
#include "Utils/Settings.h"
#include "Utils/Utils.h"

float DefaultButtonThickness()
{
    return (Utils::Max(Utils::Min(GetScreenWidth(), GetScreenHeight()) / 300.f, 2.f));
}

Color DefaultButtonBorderColour() { return {0, 0, 0, 255}; }

void Renderer::LoadTextures() noexcept
{
    for (u8 i = 0; i < (u8)COLOUR_TOTAL * (u8)TYPE_TOTAL; i++) {
        Colour    col     = Colour(i / TYPE_TOTAL);
        PieceType type    = PieceType(i % TYPE_TOTAL);
        Texture2D texture = Utils::LoadTexture(col, type, m_textureSize);
        if (IsTextureValid(texture)) {
            InfoPrintln("Renderer::LoadTextures: Loaded texture: {} {}",
                        Enums::ToString::Colour[col], Enums::ToString::Type[type]);
            m_textures[Utils::CalculateIndex(col, type)] = texture;
        } else {
            WarningPrintln("Renderer::LoadTextures: Invalid texture: {} {}",
                           Enums::ToString::Colour[col], Enums::ToString::Type[type]);
        }
    }
}

void Renderer::UnloadTextures() noexcept
{
    for (u8 i = 0; i < (u8)COLOUR_TOTAL * (u8)TYPE_TOTAL; i++) {
        Colour    col  = Colour(i / TYPE_TOTAL);
        PieceType type = PieceType(i % TYPE_TOTAL);
        u8        idx  = Utils::CalculateIndex(col, type);
        if (IsTextureValid(m_textures[idx])) {
            Utils::UnloadTexture(m_textures[idx], col, type);
            InfoPrintln("Renderer::UnloadTextures: Unloaded texture: {} {}",
                        Enums::ToString::Colour[col], Enums::ToString::Type[type]);
        }
    }
}

Renderer::Renderer()
{
    Utils::SetLogLevel(Utils::LogLevel::INFO);

    // Make texture size square
    int width     = GetScreenWidth();
    int height    = GetScreenHeight();
    m_textureSize = Utils::Min(width, height) / 8;

    // Calculate start position
    u32 sizeX = width - m_textureSize * 8;
    u32 sizeY = height - m_textureSize * 8;
    m_startX  = sizeX / 2;
    m_startY  = sizeY / 2;

    // Get textures
    LoadTextures();

    // Setup from settings
    m_dark  = Convert::U32ToColor(Settings::i(Setting::BOARD_TILE_DARK));
    m_light = Convert::U32ToColor(Settings::i(Setting::BOARD_TILE_LIGHT));
    m_promo = Convert::U32ToColor(Settings::i(Setting::BOARD_TILE_PROMO));
    m_legal = Convert::U32ToColor(Settings::i(Setting::BOARD_TILE_LEGAL));
}

Renderer::~Renderer() { UnloadTextures(); }

// ----- Read -----

// ----- Read ----- Hidden -----

int Renderer::CheckType(char cur) const noexcept
{
    switch (cur) {
    case 'b':
    case 'B':
        return static_cast<int>(BISHOP);
    case 'k':
    case 'K':
        return static_cast<int>(KING);
    case 'n':
    case 'N':
        return static_cast<int>(KNIGHT);
    case 'p':
    case 'P':
        return static_cast<int>(PAWN);
    case 'q':
    case 'Q':
        return static_cast<int>(QUEEN);
    case 'r':
    case 'R':
        return static_cast<int>(ROOK);
    default:
        return -1;
    }
}

int Renderer::CheckColour(char cur) const noexcept
{
    return static_cast<int>(isupper(cur) ? WHITE : BLACK);
}

// ----- Update -----

void Renderer::Update()
{
    FixSize();

    m_dark  = Convert::U32ToColor(Settings::i(Setting::BOARD_TILE_DARK));
    m_light = Convert::U32ToColor(Settings::i(Setting::BOARD_TILE_LIGHT));
    m_promo = Convert::U32ToColor(Settings::i(Setting::BOARD_TILE_PROMO));
    m_legal = Convert::U32ToColor(Settings::i(Setting::BOARD_TILE_LEGAL));
}

Square Renderer::Render(std::string_view fen, BitBoard moves, Square promoSquare,
                        bool isWhitePerspective) const noexcept
{
    // Render workflow

    RenderBoard();
    RenderHover();
    RenderMoves(moves, isWhitePerspective);
    RenderPieces(fen, isWhitePerspective);
    RenderPromotion(promoSquare, isWhitePerspective);

    Square sq = DetectClick(isWhitePerspective);
    return sq;
}

void Renderer::RenderMate(Colour colour, bool isCheckmate) const noexcept
{
    char  text[15];
    int   fontSize = Utils::Max(GetScreenWidth() / 50, 20);
    float spacing  = 2.f;
    Font  font     = GetFontDefault();

    if (isCheckmate) {
        snprintf(text, sizeof(text), "%s has won!", Enums::ToString::Colour[(u8)colour]);
    } else {
        strcpy(text, "Stalemate :|");
    }

    float   width  = GetScreenWidth() / 2.f;
    float   height = GetScreenHeight() / 2.f;
    Vector2 pos    = Utils::CenterText(text, font, fontSize, spacing, {width, height});

    Color dark  = Convert::U32ToColor(Settings::i(Setting::BOARD_TILE_DARK));
    Color light = Convert::U32ToColor(Settings::i(Setting::BOARD_TILE_LIGHT));

    i32       offset = 15;
    Rectangle r      = {pos.x - offset, pos.y - offset, (width - (pos.x - offset)) * 2,
                        (height - (pos.y - offset)) * 2};
    DrawRectangle(r.x, r.y, r.width, r.height, dark);
    DrawRectangleLinesEx(r, 3., light);
    DrawTextEx(GetFontDefault(), text, pos, fontSize, spacing, {255, 255, 255, 255});
}

// ----- Update ----- Hidden -----

void Renderer::FixSize()
{
    if (!IsWindowResized()) {
        return;
    }
    DebugPrintln("Renderer::FixSize: Fixing texture sizes");

    Vector3 grid  = Utils::GridPositioning();
    m_startX      = grid.x;
    m_startY      = grid.y;
    m_textureSize = grid.z;

    DebugPrintln("Renderer::FixSize: Unloading textures");
    UnloadTextures();
    DebugPrintln("Renderer::FixSize: Reloading textures");
    LoadTextures();
    DebugPrintln("Renderer::FixSize: Reloaded textures");
}

void Renderer::RenderBoard() const noexcept
{
    for (uint64_t i = 0; i < 8; i++) {
        for (uint64_t j = 0; j < 8; j++) {
            Color colour = m_dark;
            if ((i + j) % 2 == 0) {
                colour = m_light;
            }

            DrawRectangle(i * m_textureSize + m_startX, j * m_textureSize + m_startY, m_textureSize,
                          m_textureSize, colour);
        }
    }
}

void Renderer::RenderHover() const noexcept
{
    for (size_t i = 0; i < 64; i++) {
        if (IsHovered((Square)i)) {
            RenderSquare(GetHoverColour((Square)i), (Square)i, true);
        }
    }
}

void Renderer::RenderMoves(BitBoard bb, bool isWhitePerspective) const noexcept
{
    // Check the moves
    if (bb == 0) {
        return;
    }

    for (size_t i = 0; i < 64; i++) {
        Square sq = (Square)(isWhitePerspective ? i : 63 - i);

        if ((bb >> sq) & 1) {
            RenderSquare(m_legal, sq, true);
        }
    }
}

void Renderer::RenderPieces(std::string_view fen, bool isWhitePerspective) const noexcept
{
    // Easy to exit
    if (fen == "") {
        return;
    }

    // Prepares to render top to bottom or bottom to top
    int file, rank, inc;
    if (isWhitePerspective) {
        file = 0;
        rank = 0;
        inc  = 1;
    } else {
        file = 7;
        rank = 7;
        inc  = -1;
    }

    // Loop through fen
    for (size_t i = 0; i < fen.length(); i++) {
        char cur = fen[i];

        // Space means end of pieces
        if (cur == ' ') {
            break;
        }

        // Alphabetical means its a piece
        if (isalpha(cur)) {
            u8 idx = Utils::CalculateIndex(Convert::CharToColour(cur), Convert::CharToType(cur));
            std::println("Char: {}, idx: {}", cur, idx);
            RenderPiece(m_textures[idx], Square(rank * 8 + file));
            file += inc;
            continue;
        }

        // Number means increase file
        if (isdigit(cur)) {
            file += (cur - '0') * inc;
            continue;
        }

        // Change rank
        if (cur == '/') {
            file = (isWhitePerspective ? 0 : 7);
            rank += inc;
            continue;
        }

        ErrorPrintln("Renderer::RenderPieces: Invalid char detected: {}", cur);
    }
}

void Renderer::RenderPromotion(Square promotionSquare, bool isWhitePerspective) const noexcept
{
    // Validate sq
    if (!Utils::IsValidSquare(promotionSquare)) {
        return;
    }

    // Prepare data
    Square sq     = (isWhitePerspective ? promotionSquare : Square(63 - promotionSquare));
    i8     offset = (isWhitePerspective ? 8 : -8);
    Colour colour = ((promotionSquare / 8) == 0 ? BLACK : WHITE);
    if (colour == WHITE) {
        offset *= -1;
    }

    // Render the stuff
    constexpr PieceType TYPES[] = {QUEEN, ROOK, BISHOP, KNIGHT};
    for (u8 promo = 0; promo < 4; promo++) {
        Square i    = (Square)(sq + (offset * promo));
        u8     tex  = (u8)TYPES[promo] * 2 + (u8)colour;
        int    file = i % 8;
        int    rank = 7 - (i / 8);
        RenderSquare(m_promo, Square((7 - rank) * 8 + file), true);
        RenderPiece(m_textures[tex], Square(rank * 8 + file));
    }
}

// ----- Helpers -----

Square Renderer::DetectClick(bool isWhitePerspective) const noexcept
{
    for (size_t i = 0; i < 64; i++) {
        if (IsClicked((Square)i)) {
            Square sq = (Square)(isWhitePerspective ? i : 63 - i);
            return sq;
        }
    }

    return SQ_BAD;
}

Color Renderer::GetHoverColour(Square sq) const noexcept
{
    Color colour;
    if ((sq + (sq / 8)) % 2 == 0) {
        colour = m_dark;
    } else {
        colour = m_light;
    }
    colour.a = 75;
    return colour;
}

Rectangle Renderer::GetRect(Square sq) const noexcept
{
    Square file = sq % 8;
    Square rank = sq / 8;

    Vector3   grid = Utils::GridPositioning();
    Rectangle rect = {(grid.x + grid.z * float(file)), (grid.y + grid.z * (7 - rank)), (grid.z),
                      (grid.z)};
    return rect;
}

bool Renderer::IsClicked(Square sq) const noexcept
{
    return (CheckCollisionPointRec(GetMousePosition(), GetRect(sq)) &&
            IsMouseButtonPressed(MOUSE_BUTTON_LEFT));
}

bool Renderer::IsHovered(Square sq) const noexcept
{
    return CheckCollisionPointRec(GetMousePosition(), GetRect(sq));
}

void Renderer::RenderPiece(Texture2D texture, Square sq) const noexcept
{
    // Only render valid textures
    if (IsTextureValid(texture)) {
        int file = sq % 8;
        int rank = sq / 8;

        DrawTexture(texture, file * m_textureSize + m_startX, rank * m_textureSize + m_startY,
                    {255, 255, 255, 255});
    }
}

void Renderer::RenderSquare(Color colour, Square sq, bool border) const noexcept
{
    Rectangle rect = GetRect(sq);

    DrawRectangleRec(rect, colour);
    if (border) {
        DrawRectangleLinesEx(rect, DefaultButtonThickness(), DefaultButtonBorderColour());
    }
}
