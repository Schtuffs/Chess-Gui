#pragma once

#include <string>

#include "raylib.h"

/**
 * @brief Convert percentages to screen space.
 * @date 2026-06-28
 */
#define SCREEN_PERCENTAGES_2(__x, __y) \
    {(float)(GetScreenWidth() * __x), (float)(GetScreenHeight() * __y)}

/**
 * @brief Convert percentages to screen space.
 * @date 2026-06-13
 */
#define SCREEN_PERCENTAGES_4(__x, __y, __width, __height) \
    {(float)(GetScreenWidth() * __x), (float)(GetScreenHeight() * __y), \
    (float)(GetScreenWidth() * __width), (float)(GetScreenHeight() * __height)}

/**
 * @brief Container for `Button` fonts.
 * @struct FontData
 * @date 2026-06-13
 */
typedef struct FontData {
    Font font = GetFontDefault();
    Color fontColour = WHITE;
    int fontSize = 20;
} FontData;

/**
 * @brief Container for clickable button.
 * @class Button
 * @date 2026-06-13
 */
class Button {
public:
    // ----- Creation / Destruction -----

    /**
     * @brief Prepares default `Button` for rendering.
     * @date 2026-06-13
     */
    Button();
    
    /**
     * @brief Prepares `Button` for rendering.
     * @param dimensions The position and size of the `Button`.
     * @param colour The `Color` to render the `Button` with.
     * @param thickness The border thickness.
     * @date 2026-06-13
     */
    Button(const std::string& text, FontData font, Rectangle dimensions, Color colour);
    
    /**
     * @brief Free allocated memory.
     * @date 2026-06-13
     */
    ~Button();
    
    // ----- Read -----
    
    /**
     * @brief Check if the `Button` has been clicked.
     * @return `true` if the `Button` has been clicked.
     * @date 2026-06-13
     */
    bool IsClicked() const noexcept;
    
    /**
     * @brief Check if the `Button` is being hovered.
     * @return `true` if the `Button` is hovered.
     * @date 2026-06-13
     */
    bool IsHovered() const noexcept;
    
    /**
     * @brief Renders the `Button`.
     * @date 2026-06-13
     */
    void Render() const noexcept;
    
    /**
     * @brief Get the text of this `Button`.
     * @return The text
     * @date 2026-06-13
     */
    std::string Text() const noexcept;
    
    // -----Update -----
    
    /**
     * @brief Changes the `Color` of the `Button`.
     * @param newColour The `Color` to change the button to.
     * @return This `Button` for further updating.
     * @date 2026-06-13
     */
    Button& ColourInside(Color newColour);
    
    /**
     * @brief Changes the `Color` of the `Button`.
     * @param newColour The `Color` to change the `Button` to.
     * @return This `Button` for further updating.
     * @date 2026-06-13
     */
    Button& ColourBorder(Color newColour);
    
    /**
     * @brief Changes the dimensions of the `Button`.
     * @param newDimensions The new diments for the `Button`.
     * @return This `Button` for further updating.
     * @date 2026-06-13
     */
    Button& Dimension(Rectangle newDimensions);
    
    /**
     * @brief Changes the font of the text.
     * @param newFont The new text font to use.
     * @return This `Button` for further updating.
     * @date 2026-06-13
     */
    Button& Font(FontData newFont);
    
    /**
     * @brief Changes the text the `Button` displays.
     * @param newText The new text to display.
     * @return This `Button` for further updating.
     * @date 2026-06-13
     */
    Button& Text(const std::string& newText);
    
    /**
     * @brief Changes the thickness of the `Button` border.
     * @param newThickness The thickness of the border.
     * @return This `Button` for further updating.
     * @date 2026-06-13
     */
    Button& Thickness(float newThickness);

private:
    Vector2 m_textPos;
    std::string m_text;
    FontData m_font;
    Rectangle m_rect;
    Color m_colour, m_colourBorder;
    float m_borderThickness;
    bool m_isValid;
};

