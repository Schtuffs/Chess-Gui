#include "Utils.h"

#include <cstdio>

typedef struct TextureValuePair {
    char count = 0;
    Texture2D texture {};
} TextureValuePair;

static TextureValuePair textureValuePairs[12];

int CalculateIndex(Enums::Colour colour, Enums::Type type)
{
    return type * 2 + colour;
}

Vector2 Utils::CenterText(const char* text, Font font, int fontSize, Vector2 centerPoint)
{
    Vector2 textSize = MeasureTextEx(font, text, fontSize, 1.f);
    centerPoint.x = centerPoint.x - (textSize.x / 2);
    centerPoint.y = centerPoint.y - (textSize.y / 2);

    return centerPoint;
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

Texture2D Utils::LoadTexture(Enums::Colour colour, Enums::Type type, int size)
{
    // Check size
    if (size < 1) {
        TraceLog(LOG_ERROR, "Utils::LoadTexture received invalid size: %d", size);
        return Texture2D{};
    }
    
    // Get pair data
    int index = CalculateIndex(colour, type);
    TextureValuePair& pair = textureValuePairs[index];

    // Determine if should load texture
    if (pair.count == 0) {
        // Load and check image
        char path[50];
        snprintf(path, sizeof(path), "%s/%s_%s.png", PATH_PIECES, Enums::ToString::Type[type], Enums::ToString::Colour[colour]);
        Image image = LoadImage(path);
        if (!IsImageValid(image)) {
            TraceLog(LOG_ERROR, "Utils::LoadTexture Failed to load image.");
            return Texture2D{};
        }
        
        // Change image size and load to texture
        ImageResizeNN(&image, size, size);
        Texture2D texture = LoadTextureFromImage(image);
        if (!IsTextureValid(texture)) {
            TraceLog(LOG_ERROR, "Utils::LoadTexture Failed to convert image to texture.");
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
        ::UnloadTexture(texture);
        texture.id = 0;
        textureValuePairs[CalculateIndex(colour, type)].count--;
    }
}

