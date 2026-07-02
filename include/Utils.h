#pragma once

#include <cstdio>

#include "raylib.h"

#include "Constants.h"

constexpr Color BOARD_SQUARE_LIGHT          = {175, 150, 120, 255};
constexpr Color BOARD_SQUARE_DARK           = {100, 75, 60, 255};
constexpr Color BOARD_SQUARE_LIGHT_ALPHA    = {175, 150, 120, 75};
constexpr Color BOARD_SQUARE_DARK_ALPHA     = {100, 75, 60, 75};

#define UTILS_LOG_CONSOLE

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
    constexpr T Min(T a, T b) {
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
    constexpr T Max(T a, T b) {
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

    /**
     * @brief Specify the level of logging.
     * @enum LogLevel
     * @date 2026-06-21
     */
    enum class LogLevel {
        INFO,       /**< Info file. */
        DEBUG,      /**< Debug file. */
        WARNING,    /**< Warning file. */
        ERROR,      /**< Error file. */
        PRINT,      /**< Print to console. */
    };

    /**
     * @brief Don't touch this.
     * @date 2026-06-21
     */
    namespace Detail {
#ifdef UTILS_LOG_CONSOLE
        inline FILE* debugFile      = stdout;
        inline FILE* errorFile      = stdout;
        inline FILE* infoFile       = stdout;
        inline FILE* warningFile    = stdout;
#else
        inline FILE* debugFile      = fopen("debug.log", "a");
        inline FILE* errorFile      = fopen("error.log", "a");
        inline FILE* infoFile       = fopen("info.log", "a");
        inline FILE* warningFile    = fopen("warning.log", "a");
#endif

        /**
         * @brief Locks printing to prevent races.
         * @param ll The desired output stream to write to.
         * @return `true` on valid to write in this thread.
         * @author Kyle Wagler
         * @date 2026-06-20
         */
        bool LockPrint(Utils::LogLevel ll);
        
        /**
         * @brief Unlocks printing to allow another thread to print.
         * @param ll The desired output stream to stop writing to.
         * @author Kyle Wagler
         * @date 2026-06-20
         */
        void UnlockPrint(Utils::LogLevel ll);
    }

    #define FilePrintln(whichType, whichFile, initialMessage, ...) \
        if (Utils::Detail::LockPrint(whichType)) {  \
            std::print(whichFile, initialMessage);  \
            std::println(whichFile, __VA_ARGS__);   \
            Utils::Detail::UnlockPrint(whichType);  \
        } do {} while (false)
#ifdef UTILS_LOG_NONE
    #define    SyncPrintln(...) /* __VA_ARGS__ */ do {} while (false)
    #define   DebugPrintln(...) /* __VA_ARGS__ */ do {} while (false)
    #define   ErrorPrintln(...) /* __VA_ARGS__ */ do {} while (false)
    #define    InfoPrintln(...) /* __VA_ARGS__ */ do {} while (false)
    #define WarningPrintln(...) /* __VA_ARGS__ */ do {} while (false)
#else
    #define    SyncPrintln(...) FilePrintln(Utils::LogLevel::PRINT,     stdout,                     "",             __VA_ARGS__)
    #define   DebugPrintln(...) FilePrintln(Utils::LogLevel::DEBUG,     Utils::Detail::debugFile,   "DEBUG:   ",    __VA_ARGS__)
    #define   ErrorPrintln(...) FilePrintln(Utils::LogLevel::ERROR,     Utils::Detail::errorFile,   "ERROR:   ",    __VA_ARGS__)
    #define    InfoPrintln(...) FilePrintln(Utils::LogLevel::INFO,      Utils::Detail::infoFile,    "INFO:    ",    __VA_ARGS__)
    #define WarningPrintln(...) FilePrintln(Utils::LogLevel::WARNING,   Utils::Detail::warningFile, "WARNING: ",    __VA_ARGS__)
#endif

    void SetLogLevel(Utils::LogLevel ll);
}

