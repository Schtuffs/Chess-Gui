#include "Renderer.h"

#include <cctype>

#include "Convert.h"
#include "Utils.h"

static float DefaultButtonThickness()
{
    return (Utils::Max(Utils::Min(GetScreenWidth(), GetScreenHeight()) / 300.f, 2.f));
}

static void UpdateButtons(std::vector<Button>& buttons)
{
    Vector3 grid = Utils::GridPositioning();
    for (int i = 0; i < 64; i++) {
        buttons[i].Dimension(Rectangle{grid.x + grid.z * (i % GRID_SIZE), grid.y + grid.z * (GRID_SIZE - (i / GRID_SIZE) - 1), grid.z, grid.z});
        buttons[i].ColourInside((i + (i / GRID_SIZE)) % 2 == 1 ? BOARD_SQUARE_DARK_ALPHA : BOARD_SQUARE_LIGHT_ALPHA);
        buttons[i].Thickness(DefaultButtonThickness());
    }
}

static void UpdateButtonWithMove(Button& button, u64 moves, u8 index)
{
    if ((moves >> index) & 1) {
        button.ColourInside({255, 0, 0, 255});
        button.Thickness(DefaultButtonThickness());
    }
    else {
        button.ColourInside((index + (index / GRID_SIZE)) % 2 == 1 ? BOARD_SQUARE_DARK_ALPHA : BOARD_SQUARE_LIGHT_ALPHA);
        button.Thickness(0.f);
    }
}

Renderer::Renderer()
{
    // Make texture size square
    int width = GetScreenWidth();
    int height = GetScreenHeight();
    m_textureSize = Utils::Min(width, height) / GRID_SIZE;

    // Calculate start position
    u32 sizeX  = width  - m_textureSize * GRID_SIZE;
    u32 sizeY  = height - m_textureSize * GRID_SIZE;
    m_startX = sizeX / 2;
    m_startY = sizeY / 2;

    // Loop through all piece combinations and ensure texture validity
    for (u64 col = 0; col < 2; col++) {
        for (u64 type = 0; type < 6; type++) {
            Texture2D texture = Utils::LoadTexture(static_cast<Enums::Colour>(col), static_cast<Enums::Type>(type), m_textureSize);
            if (IsTextureValid(texture)) {
                int index = type * 2 + col;
                m_textures[index] = texture;
            }
            else {
                ErrorPrintln("Renderer::Renderer: Could not create texture: {} {}", Enums::ToString::Colour[col], Enums::ToString::Type[type]);
            }
        }
    }
    
    m_buttons.reserve(GRID_SIZE * GRID_SIZE);
    Vector3 grid = Utils::GridPositioning();
    for (u64 rank = 0; rank < GRID_SIZE; rank++) {
        for (u64 file = 0; file < GRID_SIZE; file++) {
            Color col = (((rank + file) % 2) == 0 ? BOARD_SQUARE_DARK_ALPHA : BOARD_SQUARE_LIGHT_ALPHA);
            m_buttons.emplace_back("", FontData{}, Rectangle{grid.x + grid.z * file, grid.y + grid.z * (GRID_SIZE - rank - 1), grid.z, grid.z}, col);
            m_buttons.back().Thickness(DefaultButtonThickness());
        }
    }
}

Renderer::~Renderer()
{
    // Only free valid textures
    for (uint64_t col = 0; col < 2; col++) {
        for (uint64_t type = 0; type < 6; type++) {
            int i = type * 2 + col;
            if (IsTextureValid(m_textures[i])) {
                Utils::UnloadTexture(m_textures[i], static_cast<Enums::Colour>(col), static_cast<Enums::Type>(type));
            }
        }
    }
}



// ----- Read -----

std::string Renderer::CheckMove(bool isWhitePerspective) const noexcept
{
    std::string move;

    for (size_t i = 0; i < m_buttons.size(); i++) {
        u8 index = (u8)(isWhitePerspective ? i : 63 - i);
        const Button& button = m_buttons[index];

        // Player is making moves
        if (button.IsClicked()) {
            move = Convert::IndexToMove(index);
        }
    }

    return move;
}

void Renderer::RenderBoard(Color dark, Color light) const noexcept
{
    for (uint64_t i = 0; i < GRID_SIZE; i++) {
        for (uint64_t j = 0; j < GRID_SIZE; j++) {
            Color colour = dark;
            if ((i + j) % 2 == 0) {
                colour = light;
            }
    
            DrawRectangle(i * m_textureSize + m_startX, j * m_textureSize + m_startY, m_textureSize, m_textureSize, colour);
        }
    }
}

void Renderer::RenderMoves(BitBoard bb, bool isWhitePerspective)
{
    for (size_t i = 0; i < m_buttons.size(); i++) {
        u8 index = (u8)(isWhitePerspective ? i : 63 - i);
        Button& button = m_buttons[i];
        UpdateButtonWithMove(button, bb, index);
        
        if (button.IsHovered()) {
            button.Thickness(DefaultButtonThickness());
            button.Render();
        }

        if ((bb >> index) & 1){ 
            button.Render();
        }
    }
}

void Renderer::RenderPieces(std::string_view fen, bool isWhitePerspective) const noexcept
{
    // Prepares to render top to bottom or bottom to top
    int file, rank, inc;
    if (isWhitePerspective) {
        file = 0;
        rank = 0;
        inc = 1;
    }
    else {
        file = GRID_SIZE - 1;
        rank = GRID_SIZE - 1;
        inc = -1;
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
            int type = CheckType(cur);
            int colour = CheckColour(cur);
            RenderPiece(m_textures[type * 2 + colour], {file * m_textureSize + m_startX, rank * m_textureSize + m_startY});
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
            file = (isWhitePerspective ? 0 : (GRID_SIZE - 1));
            rank += inc;
            continue;
        }

        ErrorPrintln("Renderer::RenderPieces: Invalid char detected: {}", cur);
    }
}



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

void Renderer::RenderPiece(Texture2D texture, Vec2<int> pos) const noexcept
{
    // Only render valid textures
    if (IsTextureValid(texture)) {
        DrawTexture(texture, pos.x, pos.y, WHITE);
    }
}



// ----- Update -----

void Renderer::FixSize()
{
    if (!IsWindowResized()) {
        return;
    }
    DebugPrintln("Fixing");

    Vector3 grid = Utils::GridPositioning();
    m_startX = grid.x;
    m_startY = grid.y;
    m_textureSize = grid.z;
    
    DebugPrintln("Unloading");
    for (uint64_t col = 0; col < 2; col++) {
        for (uint64_t type = 0; type < 6; type++) {
            int i = type * 2 + col;
            if (IsTextureValid(m_textures[i])) {
                Utils::UnloadTexture(m_textures[i], static_cast<Enums::Colour>(col), static_cast<Enums::Type>(type));
            }
        }
    }

    DebugPrintln("Reloading");
    for (u64 col = 0; col < 2; col++) {
        for (u64 type = 0; type < 6; type++) {
            Texture2D texture = Utils::LoadTexture(static_cast<Enums::Colour>(col), static_cast<Enums::Type>(type), m_textureSize);
            if (IsTextureValid(texture)) {
                int index = type * 2 + col;
                m_textures[index] = texture;
            }
            else {
                ErrorPrintln("Renderer::Renderer: Could not create texture: {} {}", Enums::ToString::Colour[col], Enums::ToString::Type[type]);
            }
        }
    }
    DebugPrintln("Reloaded");

    UpdateButtons(m_buttons);
}

