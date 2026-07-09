#pragma once

#include <cstdint>
#include <string_view>
#include <vector>

#include "raylib.h"

#include "Button.h"
#include "Constants.h"

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
    
    /**
     * @brief Check if player clicked board and attempted to make a move.
     * @param isWhitePerspective Determines if checking clicks from whites perspective or blacks perspective.
     * @return The grid square the player clicked or "".
     * @date 2026-06-11
     */
    std::string CheckMove(bool isWhitePerspective) const noexcept;

    /**
     * @brief Renders the board for the background.
     * @param dark The `Color` for the dark tiles.
     * @param light The `Color` for the light tiles.
     * @date 2026-06-11
     */
    void RenderBoard(Color dark, Color light) const noexcept;

    /**
     * @brief Renders the potential moves.
     * @param moves The `BitBoard` representation of the valid moves for a `Piece`.
     * @param isWhitePerspective Determines if rendering moves from whites perspective or blacks perspective.
     * @date 2026-07-08
     */
    void RenderMoves(BitBoard moves, bool isWhitePerspective);
    
    /**
     * @brief Renders the pieces based on a fen.
     * @param fen The fen state to render the board in.
     * @param isWhitePerspective Determines if rendering pieces from whites perspective or blacks perspective.
     * @date 2026-06-11
     */
    void RenderPieces(std::string_view fen, bool isWhitePerspective) const noexcept;
    
    // ----- Update -----
    
    /**
     * @brief Fixes window and UI elements size.
     * @date 2026-06-15
     */
    void FixSize();

private:
    Texture2D m_textures[12];
    std::vector<Button> m_buttons;
    int m_textureSize, m_startX, m_startY;
    
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
    
    
    /**
     * @brief Renders a piece to the board.
     * @param texture The `Texture2D` to render.
     * @param pos The `Vec2` position of the piece.
     * @date 2026-06-11
     */
    void RenderPiece(Texture2D texture, Vec2<int> pos) const noexcept;
};

