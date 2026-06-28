#pragma once

#include "raylib.h"

#include "Constants.h"

/**
 * @brief Utility functions.
 * @namespace Utils
 * @date 2026-06-06
 */
namespace Utils {
    /**
     * @brief Calculate the minimum of 2 values of type `T`.
     * @param a The first value.
     * @param b The second value.
     * @return The smaller of the 2. Returns value `a` on equivalent.
     * @date 2026-06-06
     */
    template <typename T>
    T Min(T a, T b) {
        return (b < a) ? b : a;
    }
    /**
     * @brief Calculate the maximum of 2 values of type `T`.
     * @param a The first value.
     * @param b The second value.
     * @return The larger of the 2. Returns value `a` on equivalent.
     * @date 2026-06-06
     */
    template <typename T>
    T Max(T a, T b) {
        return (b > a) ? b : a;
    }

    /**
     * @brief Creates a clickable button that only gets clicked on press and release hovering.
     * @param rect The area to render the button.
     * @param text The text to center.
     * @param id The ID of the button to determine clicking status.
     * @return `true` when button was hovered on both click and release.
     * @date 2026-06-15
     */
    bool ClickableButton(Rectangle rect, const char* text, u8 id);
    /**
     * @brief Centers given text.
     * @param text The text to center.
     * @param font The text `Font`.
     * @return The position to draw the text at for it to be centered.
     * @date 2026-06-15
     */
    Vector2 CenterText(const char* text, Font font, int fontSize, Vector2 centerPoint);
    /**
     * @brief Centers given text.
     * @return The position of the first square with {x, y, size}.
     * @date 2026-06-15
     */
    Vector3 GridPositioning();
    /**
     * @brief Calculates the start position for a button within the grid on screen.
     * @return The position of the first square with {x, y, size}.
     * @date 2026-06-15
     */
    Rectangle StartButtonPos(u8 x, u8 y, u8 width, u8 height);
    /**
     * @brief Loads a `Texture2D` to the GPU.
     * @param `Enums::Colour` The `Piece` colour.
     * @param `Enums::Type` The `Piece` type.
     * @return The loaded `Texture2D`. Check with `IsTextureValid(Texture2D)`.
     * @date 2026-06-06
     */
    Texture2D LoadTexture(Enums::Colour colour, Enums::Type type, int size);
    /**
     * @brief Unloads a `Texture2D` from the GPU.
     * @param `Texture2D` The texture to unload.
     * @param `Enums::Colour` The `Piece` colour.
     * @param `Enums::Type` The `Piece` type.
     * @date 2026-06-06
     */
    void UnloadTexture(Texture2D& texture, Enums::Colour colour, Enums::Type type);
}

