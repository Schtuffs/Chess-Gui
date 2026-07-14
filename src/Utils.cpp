#include "Utils.h"

#include <atomic>
#include <cstdio>
#include <mutex>
#include <print>

#include "raygui.h"

typedef struct TextureValuePair {
    u8 count = 0;
    Texture2D texture {};
} TextureValuePair;

static TextureValuePair s_textureValuePairs[12];
static u8 s_clickedButton = 0;

int CalculateIndex(Enums::Colour colour, Enums::Type type)
{
    return ((int)type * 2) + (int)colour;
}

Vector2 Utils::CenterText(const char* text, Font font, int fontSize, Vector2 centerPoint)
{
    Vector2 textSize = MeasureTextEx(font, text, fontSize, 1.f);
    centerPoint.x = centerPoint.x - (textSize.x / 2);
    centerPoint.y = centerPoint.y - (textSize.y / 2);

    return centerPoint;
}

bool Utils::ClickableButton(Rectangle rect, const char* text, u8 id)
{
    Vector2 mousePos = GetMousePosition();
    bool isHovered = CheckCollisionPointRec(mousePos, rect);

    GuiButton(rect, text);

    if (isHovered && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        s_clickedButton = id;
    }

    if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
        if (isHovered && s_clickedButton == id) {
            s_clickedButton = 0;
            return true;
        }
        if (s_clickedButton == id) {
            s_clickedButton = 0;
        }
    }

    return false;
}

Vector3 Utils::GridPositioning()
{
    // Make square
    int width = GetScreenWidth();
    int height = GetScreenHeight();
    float size = Utils::Min(width, height) / GRID_SIZE;

    // Calculate start position
    int sizeX  = width  - size * GRID_SIZE;
    int sizeY  = height - size * GRID_SIZE;
    float startX = sizeX / 2;
    float startY = sizeY / 2;

    return { startX, startY, size };
}

Rectangle Utils::ButtonPos(u8 x, u8 y, u8 width, u8 height)
{
    Vector3 grid = Utils::GridPositioning();
    Rectangle pos = {grid.x + grid.z * 0.125f, grid.y + grid.z * 0.125f,
        grid.z * 0.75f, grid.z * 0.75f};

    pos.x += x * grid.z;
    pos.y += y * grid.z;
    pos.width  += (width - 1)  * grid.z;
    pos.height += (height - 1) * grid.z;
    
    return pos;
}

Texture2D Utils::LoadTexture(Enums::Colour colour, Enums::Type type, int size)
{
    // Check size
    if (size < 1) {
        ErrorPrintln("Utils::LoadTexture: Received invalid size: {}", size);
        return Texture2D{};
    }
    
    // Get pair data
    int index = CalculateIndex(colour, type);
    TextureValuePair& pair = s_textureValuePairs[index];

    // Determine if should load texture
    if (pair.count == 0) {
        // Load and check image
        char path[50];
        snprintf(path, sizeof(path), "%s/%s_%s.png", PATH_PIECES.data(), Enums::ToString::Type[(int)type], Enums::ToString::Colour[(int)colour]);
        Image image = LoadImage(path);
        if (!IsImageValid(image)) {
            ErrorPrintln("Utils::LoadTexture: Failed to load image.");
            return Texture2D{};
        }
        
        // Change image size and load to texture
        ImageResizeNN(&image, size, size);
        Texture2D texture = LoadTextureFromImage(image);
        if (!IsTextureValid(texture)) {
            ErrorPrintln("Utils::LoadTexture Failed to convert image to texture.");
            return Texture2D{};
        }

        pair.texture = texture;
        UnloadImage(image);
        SetTextureFilter(pair.texture, TEXTURE_FILTER_POINT);
    }

    // Account and return
    pair.count++;
    return pair.texture;
}

void Utils::UnloadTexture(Texture2D& texture, Enums::Colour colour, Enums::Type type)
{
    if (IsTextureValid(texture)) {
        int index = CalculateIndex(colour, type);
        TextureValuePair& pair = s_textureValuePairs[index];
        pair.count--;
        
        if (pair.count == 0) {
            ::UnloadTexture(texture);
            texture.id = 0;
        }
    }
}

static std::mutex mtxPrint, mtxDebug, mtxError, mtxInfo, mtxWarning;
static std::atomic<Utils::LogLevel> s_logLevel = Utils::LogLevel::ERROR;

bool Utils::Detail::LockPrint(Utils::LogLevel ll)
{
    if (ll < s_logLevel) {
        return false;
    }

#ifdef FILES_ALL_CONSOLE
    mtxPrint.lock();
    return true;
#else
    switch (ll) {
    case Utils::LogLevel::INFO:
        mtxInfo.lock();
        return true;
    case Utils::LogLevel::DEBUG:
        mtxDebug.lock();
        return true;
    case Utils::LogLevel::WARNING:
        mtxWarning.lock();
        return true;
    case Utils::LogLevel::ERROR:
        mtxError.lock();
        return true;
    case Utils::LogLevel::PRINT:
        mtxPrint.lock();
        return true;
    default:
        ErrorPrintln("Invalid filetype lock: {}", (int)ll);
        return false;
    }
#endif
}

void Utils::Detail::UnlockPrint(Utils::LogLevel ll)
{
#ifdef FILES_ALL_CONSOLE
    (void)ll;
    mtxPrint.unlock();
#else
    switch (ll) {
    case Utils::LogLevel::DEBUG:
        mtxDebug.unlock();
        break;
    case Utils::LogLevel::ERROR:
        mtxError.unlock();
        break;
    case Utils::LogLevel::INFO:
        mtxInfo.unlock();
        break;
    case Utils::LogLevel::PRINT:
        mtxPrint.unlock();
        break;
    case Utils::LogLevel::WARNING:
        mtxWarning.unlock();
        break;
    default:
        ErrorPrintln("Invalid filetype unlock: {}", (int)ll);
        break;
    }
#endif
}

void Utils::SetLogLevel(Utils::LogLevel ll)
{
    s_logLevel = ll;
}

