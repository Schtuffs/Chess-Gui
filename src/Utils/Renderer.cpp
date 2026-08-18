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

Color DefaultButtonBorderColour() { return BLACK; }

Renderer::Renderer()
{
    // Make texture size square
    int width     = GetScreenWidth();
    int height    = GetScreenHeight();
    m_textureSize = Utils::Min(width, height) / 8;

    // Calculate start position
    u32 sizeX = width - m_textureSize * 8;
    u32 sizeY = height - m_textureSize * 8;
    m_startX  = sizeX / 2;
    m_startY  = sizeY / 2;

    // Loop through all piece combinations and ensure texture validity
    for (u64 col = 0; col < 2; col++) {
        for (u64 type = 0; type < 6; type++) {
            Texture2D texture = Utils::LoadTexture(static_cast<Enums::Colour>(col),
                                                   static_cast<Enums::Type>(type), m_textureSize);
            if (IsTextureValid(texture)) {
                InfoPrintln("Renderer::Renderer: Loaded texture: {} {}",
                            Enums::ToString::Colour[col], Enums::ToString::Type[type]);
                int index         = type * 2 + col;
                m_textures[index] = texture;
            } else {
                WarningPrintln("Renderer::Renderer: Invalid texture: {} {}",
                               Enums::ToString::Colour[col], Enums::ToString::Type[type]);
            }
        }
    }

    m_dark  = Convert::U32ToColor(Settings::i(Setting::BOARD_TILE_DARK));
    m_light = Convert::U32ToColor(Settings::i(Setting::BOARD_TILE_LIGHT));
    m_promo = Convert::U32ToColor(Settings::i(Setting::BOARD_TILE_PROMO));
    m_legal = Convert::U32ToColor(Settings::i(Setting::BOARD_TILE_LEGAL));
}

Renderer::~Renderer()
{
    // Only free valid textures
    for (uint64_t col = 0; col < 2; col++) {
        for (uint64_t type = 0; type < 6; type++) {
            int i = type * 2 + col;
            if (IsTextureValid(m_textures[i])) {
                Utils::UnloadTexture(m_textures[i], static_cast<Enums::Colour>(col),
                                     static_cast<Enums::Type>(type));
            }
        }
    }
}

// ----- Read -----

// ----- Read ----- Hidden -----

int Renderer::CheckType(char cur) const noexcept
{
    switch (cur) {
    case 'b':
    case 'B':
        return static_cast<int>(Enums::Type::Bishop);
    case 'k':
    case 'K':
        return static_cast<int>(Enums::Type::King);
    case 'n':
    case 'N':
        return static_cast<int>(Enums::Type::Knight);
    case 'q':
    case 'Q':
        return static_cast<int>(Enums::Type::Queen);
    case 'p':
    case 'P':
        return static_cast<int>(Enums::Type::Pawn);
    case 'r':
    case 'R':
        return static_cast<int>(Enums::Type::Rook);
    default:
        return -1;
    }
}

int Renderer::CheckColour(char cur) const noexcept
{
    return static_cast<int>(isupper(cur) ? Enums::Colour::White : Enums::Colour::Black);
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

std::string Renderer::Render(std::string_view fen, BitBoard moves, Index promoSquare,
                             bool isWhitePerspective) const noexcept
{
    // Render workflow

    RenderBoard();
    RenderHover();
    RenderMoves(moves, isWhitePerspective);
    RenderPieces(fen, isWhitePerspective);
    RenderPromotion(promoSquare, isWhitePerspective);

    Index       index = DetectClick(isWhitePerspective);
    std::string move  = Convert::IndexToMove(index);
    return move;
}

void Renderer::RenderMate(Enums::Colour colour, bool isCheckmate) const noexcept
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
    DrawTextEx(GetFontDefault(), text, pos, fontSize, spacing, WHITE);
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
    for (uint64_t col = 0; col < 2; col++) {
        for (uint64_t type = 0; type < 6; type++) {
            int i = type * 2 + col;
            if (IsTextureValid(m_textures[i])) {
                Utils::UnloadTexture(m_textures[i], static_cast<Enums::Colour>(col),
                                     static_cast<Enums::Type>(type));
            }
        }
    }

    DebugPrintln("Renderer::FixSize: Reloading textures");
    for (u64 col = 0; col < 2; col++) {
        for (u64 type = 0; type < 6; type++) {
            Texture2D texture = Utils::LoadTexture(static_cast<Enums::Colour>(col),
                                                   static_cast<Enums::Type>(type), m_textureSize);
            if (IsTextureValid(texture)) {
                int index         = type * 2 + col;
                m_textures[index] = texture;
            } else {
                ErrorPrintln("Renderer::FixSize: Could not create texture: {} {}",
                             Enums::ToString::Colour[col], Enums::ToString::Type[type]);
            }
        }
    }
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
        if (IsHovered(i)) {
            RenderSquare(GetHoverColour(i), i, true);
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
        Index index = (Index)(isWhitePerspective ? i : 63 - i);

        if ((bb >> index) & 1) {
            RenderSquare(m_legal, index, true);
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
            int type   = CheckType(cur);
            int colour = CheckColour(cur);
            RenderPiece(m_textures[type * 2 + colour], rank * 8 + file);
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

void Renderer::RenderPromotion(Index promotionSquare, bool isWhitePerspective) const noexcept
{
    // Validate index
    if (!Utils::IsValidIndex(promotionSquare)) {
        return;
    }

    // Prepare data
    Index         index  = (isWhitePerspective ? promotionSquare : 63 - promotionSquare);
    i8            offset = (isWhitePerspective ? 8 : -8);
    Enums::Colour colour =
        ((promotionSquare / 8) == 0 ? Enums::Colour::Black : Enums::Colour::White);
    if (colour == Enums::Colour::White) {
        offset *= -1;
    }

    // Render the stuff
    constexpr Enums::Type TYPES[] = {Enums::Type::Queen, Enums::Type::Rook, Enums::Type::Bishop,
                                     Enums::Type::Knight};
    for (u8 promo = 0; promo < 4; promo++) {
        Index i    = index + (offset * promo);
        Index tex  = (u8)TYPES[promo] * 2 + (u8)colour;
        int   file = i % 8;
        int   rank = 7 - (i / 8);
        RenderSquare(m_promo, (7 - rank) * 8 + file, true);
        RenderPiece(m_textures[tex], rank * 8 + file);
    }
}

// ----- Helpers -----

Index Renderer::DetectClick(bool isWhitePerspective) const noexcept
{
    for (size_t i = 0; i < 64; i++) {
        if (IsClicked(i)) {
            Index index = (Index)(isWhitePerspective ? i : 63 - i);
            return index;
        }
    }

    return 64;
}

Color Renderer::GetHoverColour(Index index) const noexcept
{
    Color colour;
    if ((index + (index / 8)) % 2 == 0) {
        colour = m_dark;
    } else {
        colour = m_light;
    }
    colour.a = 75;
    return colour;
}

Rectangle Renderer::GetRect(Index index) const noexcept
{
    Index file = index % 8;
    Index rank = index / 8;

    Vector3   grid = Utils::GridPositioning();
    Rectangle rect = {(grid.x + grid.z * file), (grid.y + grid.z * (7 - rank)), (grid.z), (grid.z)};
    return rect;
}

bool Renderer::IsClicked(Index index) const noexcept
{
    return (CheckCollisionPointRec(GetMousePosition(), GetRect(index)) &&
            IsMouseButtonPressed(MOUSE_BUTTON_LEFT));
}

bool Renderer::IsHovered(Index index) const noexcept
{
    return CheckCollisionPointRec(GetMousePosition(), GetRect(index));
}

void Renderer::RenderPiece(Texture2D texture, Index index) const noexcept
{
    // Only render valid textures
    if (IsTextureValid(texture)) {
        int file = index % 8;
        int rank = index / 8;

        DrawTexture(texture, file * m_textureSize + m_startX, rank * m_textureSize + m_startY,
                    WHITE);
    }
}

void Renderer::RenderSquare(Color colour, Index index, bool border) const noexcept
{
    Rectangle rect = GetRect(index);

    DrawRectangleRec(rect, colour);
    if (border) {
        DrawRectangleLinesEx(rect, DefaultButtonThickness(), DefaultButtonBorderColour());
    }
}
