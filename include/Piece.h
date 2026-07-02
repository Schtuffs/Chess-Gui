#pragma once

#include <string>

#include "raylib.h"

#include "Constants.h"

/**
 * @brief The main piece class.
 * @class Piece
 * @date 2026-06-08
 */
class Piece {
public:
    // ----- Creation / Destruction -----

    /**
     * @brief Creates an invalid piece.
     * @date 2026-06-28
     */
    Piece();
    
    /**
     * @brief Creates a piece with a given type and colour.
     * @param `Colour` The `Colour` of the piece.
     * @param `Type` The `Type` of the piece.
     * @date 2026-06-08
     */
    Piece(Enums::Colour colour, Enums::Type type, u8 pos);
    
    /**
     * @brief Frees any allocated memory associated with the piece.
     * @date 2026-06-08
     */
    ~Piece();

    void Position(u8);

    Enums::Colour Colour() const noexcept;
    Enums::Type Type() const noexcept;
    u64 Position() const noexcept;
    bool IsValid() const noexcept;

    std::string ToString() const noexcept;
    char AsChar() const noexcept;

private:
    Enums::Colour m_colour; 
    Enums::Type m_type;
    u8 m_pos;
    bool m_isValid;
};

