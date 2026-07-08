#pragma once

#include <cstdint>
#include <string_view>

using i8        =   int8_t;
using u8        =  uint8_t;
using i16       =  int16_t;
using u16       = uint16_t;
using i32       =  int32_t;
using u32       = uint32_t;
using i64       =  int64_t;
using u64       = uint64_t;
using Index     =       u8;
using BitBoard  =      u64;

/**
 * @brief Helpful for holding 2 variables of the same type in 1.
 * @tparam T The desired datatype.
 * @date 2026-06-11
 */
template <typename T>
struct Vec2 {
    T x, y;
};

/**
 * @brief Default fen string.
 * @date 2026-06-11
 */
constexpr std::string_view DEFAULT_FEN  = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

/**
 * @brief Grid size in files and ranks.
 * @date 2026-06-11
 */
constexpr u64 GRID_SIZE = 8;

/**
 * @brief Resource path for piece textures.
 * @date 2026-06-08
 */
constexpr std::string_view PATH_PIECES  = "../resources/pieces";

/**
 * @brief Holds global enums.
 * @namespace Enums
 * @date 2026-06-08
 */
namespace Enums {
    /**
     * @brief Holds information for castling rights.
     * @enum Castling
     * @date 2026-07-03
     */
    enum class Castling {
        Black_King  = 1,    /**< Black castling kingside */
        Black_Queen = 2,    /**< Black castling queenside */
        White_King  = 4,    /**< White castling kingside */
        White_Queen = 8,    /**< White castling queenside */
    };

    /**
     * @brief Holds information for the colour of the piece.
     * @enum Colour
     * @date 2026-06-08
     */
    enum class Colour {
        Black,      /**< Piece colour black */
        White,      /**< Piece colour white */
        Invalid,    /**< Invalid piece. */
    };
    
    /**
     * @brief Screen state information.
     * @enum Screen
     * @date 2026-06-08
     */
    enum class Screen {
        Quit,           /**< Quitting the game. */
        Menu,           /**< Main menu screen. */
        NewGame,        /**< New game options screen. */
        Multiplayer,    /**< Setup multiplayer screen. */
        Game,           /**< Playing a game screen. */
        Settings,       /**< Settings screen. */
    };
    
    /**
     * @brief Holds information for the type of piece.
     * @enum Type
     * @date 2026-06-08
     */
    enum class Type {
        Bishop,     /**< Bishop type. */
        King,       /**< King type. */
        Knight,     /**< Knight type. */
        Pawn,       /**< Pawn type. */
        Queen,      /**< Queen type. */
        Rook,       /**< Rook type. */
        Invalid,    /**< Invalid piece. */
    };
    
    /**
     * @brief Convert an enum variable to a string via indexing with enum value.
     * @namespace ToString
     * @date 2026-06-08
     */
    namespace ToString {
        /**
         * @brief Convert `Enums::Colour` to string.
         * @date 2026-06-08
         */
        constexpr const char* Colour[] = {
            "black", "white", "invalid",
        };
        
        /**
         * @brief Convert `Enums::Type` to string.
         * @date 2026-06-08
         */
        constexpr const char* Type[] = {
            "bishop", "king", "knight",
            "pawn", "queen", "rook",
            "invalid",
        };
        
        /**
         * @brief Convert `Utils::Setting` to string.
         * @date 2026-07-01
         */
        constexpr const char* Setting[] = {
            "SAVE_GAME_STATE", "GAME_STATE",
            "BOARD_TILE_DARK", "BOARD_TILE_LIGHT",
            "TOTAL_SETTINGS",
        };
    }
}

