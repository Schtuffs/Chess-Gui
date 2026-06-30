#include "Fen.h"

#include <sstream>
#include <string>

#include "Fen.h"

#include <print>

#include "Utils.h"



// ----- Hidden -----

/**
 * Moves string_view forward to space.
 */
bool MoveToSpace(std::string_view& view)
{
    size_t index = view.find(' ');
    if (index == std::string_view::npos || index + 1 >= view.length()) {
        return false;
    }
    view = view.substr(index + 1);
    return true;
}

/**
 * Validates piece data in given fen string.
 */
bool ValidatePieces(std::string_view fen)
{
    // Check files and ranks
    i8 files = 0, ranks = 0;
    size_t index = 0;
    while (index < fen.size()) {
        char c = fen[index];
    
        // Marks end of rank data, validate
        if (c == '/') {
            if (files != 8) {
                WarningPrintln("Invalid number of files in fen: {}", files);
                return false;
            }
            files = 0;
            ranks++;
        }
    
        // Marks end of all ranks, validate
        else if (c == ' ') {
            if (files != 8 && ranks != 8) {
                WarningPrintln("Invalid number of files or ranks in fen: {}, {}", files, ranks);
                return false;
            }
            break;
        }
    
        else if (
            c == 'b' || c == 'B' ||
            c == 'k' || c == 'K' ||
            c == 'n' || c == 'N' ||
            c == 'p' || c == 'P' ||
            c == 'q' || c == 'Q' ||
            c == 'r' || c == 'R'
        ) {
            files++;
        }
    
        else if (isdigit(c)) {
            files += c - '0';
        }

        else {
            WarningPrintln("Invalid char in fen: {}", c);
            return false;
        }
    
        index++;
    }

    return true;
}

/**
 * Validates the current player to move.
 */
bool ValidateMove(std::string_view fen)
{
    char c = fen[0];
    if (fen.length() != 1 && c != 'w' && c != 'b') {
        WarningPrintln("Invalid player-to-move argument in fen: {}", c);
        return false;
    }

    return true;
}

/**
 * Validates castling data.
 */
bool ValidateCastling(std::string_view fen)
{
    if (fen.length() > 4) {
        return false;
    }

    if (fen[0] == '-') {
        return true;
    }

    constexpr std::string_view str = "KQkq";
    u8 strPos = 0;
    for (char c : fen) {
        while (strPos < str.length() && c != str[strPos]) {
            strPos++;
        }
        
        if (strPos == str.length()) {
            ErrorPrintln("Invalid castling data in fen: {}", fen);
            return false;
        }
        strPos++;
    }
    
    return true;
}

/**
 * Validates en passant square.
 */
bool ValidateEnPassant(std::string_view fen)
{
    if (fen[0] == '-') {
        return true;
    }

    if (fen.length() != 2) {
        return false;
    }

    char c = fen[0];
    if ('a' > c || c > 'h') {
        ErrorPrintln("Invalid file in fen: {}", c);
        return false;
    }
    
    c = fen[1];
    if (c != '3' && c != '6') {
        ErrorPrintln("Invalid rank in fen: {}", c);
        return false;
    }

    return true;
}

bool NextCheck(std::istringstream& ss, bool(*validate)(std::string_view))
{
    std::string str;
    ss >> str;
    if (ss.fail() || !validate(str)) {
        return false;
    }
    return true;
}

bool Fen::IsValidFen(const char* data)
{
    std::istringstream ss(data);
    if (!NextCheck(ss, ValidatePieces)) {
        ErrorPrintln("Invalid pieces in fen");
        return false;
    }
    
    if (!NextCheck(ss, ValidateMove)) {
        ErrorPrintln("Invalid player to move in fen");
        return false;
    }
    
    if (!NextCheck(ss, ValidateCastling)) {
        ErrorPrintln("Invalid castling rights in fen");
        return false;
    }
    
    if (!NextCheck(ss, ValidateEnPassant)) {
        ErrorPrintln("Invalid en passant in fen");
        return false;
    }
    
    return true;
}

std::string Fen::IndexToMove(u8 index)
{
    return ((char)((index % GRID_SIZE) + 'a') + std::to_string((index / GRID_SIZE) + 1));
}

u8 Fen::MoveToIndex(std::string_view move)
{
    return (u8)((move[0] - 'a') * GRID_SIZE) + (move[1] - '0');
}

