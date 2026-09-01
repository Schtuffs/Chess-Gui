#pragma once

#include <cstdint>
#include <string_view>
#include <vector>

#include "raylib.h"
#undef WHITE
#undef BLACK

#include "Types/BitBoard.h"
#include "Types/Types.h"
#include "Utils/Constants.h"

/**
 * @brief Handles the rendering to the screen.
 * @class Renderer
 * @date 2026-06-11
 */
class Renderer {
public:
    // ----- Creation / Destruction -----

    /**
     * @brief Loads `Texture2D` from resources.
     * @date 2026-06-11
     */
    Renderer();
    /**
     * @brief Cleanup loaded `Texture2D`.
     * @date 2026-06-11
     */
    ~Renderer();

    // ----- Read -----

    void   Update();
    Square Render(std::string_view fen, BitBoard moves, Square promoSquare,
                  bool isWhitePerspective) const noexcept;
    void   RenderMate(Colour winner, bool isCheckmate) const noexcept;
    void   RenderSquare(Color colour, Square sq, bool border) const noexcept;

private:
    Texture2D m_textures[TYPE_TOTAL * 2];
    Color     m_dark, m_light, m_promo, m_legal;
    int       m_textureSize, m_startX, m_startY;

    // Render workflow

    void LoadTextures() noexcept;
    void UnloadTextures() noexcept;

    void RenderBoard() const noexcept;
    void RenderHover() const noexcept;
    void RenderMoves(BitBoard moves, bool isWhitePerspective) const noexcept;
    void RenderPieces(std::string_view fen, bool isWhitePerspective) const noexcept;
    void RenderPromotion(Square sq, bool isWhitePerspective) const noexcept;

    // Helper functions

    Square    DetectClick(bool isWhitePerspective) const noexcept;
    void      FixSize();
    Rectangle GetRect(Square sq) const noexcept;
    Color     GetHoverColour(Square sq) const noexcept;
    bool      IsClicked(Square sq) const noexcept;
    bool      IsHovered(Square sq) const noexcept;
    void      RenderPiece(Texture2D texture, Square sq) const noexcept;

    /**
     * @brief Determines a pieces `Color`.
     * @param cur The current char in question.
     * @return The `Color` of the piece;
     * @date 2026-06-11
     */
    int CheckColour(char cur) const noexcept;

    /**
     * @brief Determines a pieces type.
     * @param cur The current char in question.
     * @return The type of the piece
     * @date 2026-06-11
     */
    int CheckType(char cur) const noexcept;
};
