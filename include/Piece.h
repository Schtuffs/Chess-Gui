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
    
    /**
     * @brief Frees any allocated memory associated with the piece.
     * @date 2026-06-08
     */
    void Position(u8);
    
    /**
     * @brief Get the `Enums::Colour` of the `Piece`.
     * @return The `Enums::Colour` of the `Piece`.
     * @date 2026-06-29
     */
    Enums::Colour Colour() const noexcept;

    /**
     * @brief Check if `Piece` is valid.
     * @return `true` on valid `Piece`.
     * @date 2026-06-29
     */
    bool IsValid() const noexcept;
    
    /**
     * @brief Get the bitboard position of the `Piece`.
     * @return The position of the `Piece`.
     * @date 2026-06-29
     */
    u64 Position() const noexcept;
    
    /**
     * @brief Get the `Enums::Type` of the `Piece`.
     * @return The `Enums::Type` of the `Piece`.
     * @date 2026-06-29
     */
    Enums::Type Type() const noexcept;

    /**
     * @brief Get the char representation of the `Piece`.
     * @return The char representation of the `Piece`.
     * @date 2026-06-29
     */
    char AsChar() const noexcept;
    
    /**
     * @brief Get the string representation of the `Piece`.
     * @return The string representation of the `Piece`.
     * @date 2026-06-29
     */
    std::string ToString() const noexcept;

private:
    Enums::Colour m_colour; 
    Enums::Type m_type;
    u8 m_pos;
    bool m_isValid;
};

