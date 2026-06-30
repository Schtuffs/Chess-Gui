#include "Board.h"

#include <print>

#include "Fen.h"
#include "MoveGen.h"
#include "Utils.h"



// ----- Creation ----- Destruction -----

Board::Board(std::string_view fen)
    : m_fen(fen)
{
    if (!Fen::IsValidFen(m_fen.c_str())) {
        ErrorPrintln("Invalid fen: {}", m_fen);
        m_fen = DEFAULT_FEN;
    }
    DebugPrintln("Fen: {}", m_fen);
    
    u8 index = 0;
    for (u64 rank = GRID_SIZE - 1; rank < GRID_SIZE; rank--) {
        for (u64 file = 0; file < GRID_SIZE; file++) {
            u64 i = rank * GRID_SIZE + file;
            char c = fen[index++];

            if (isdigit(c)) {
                file += c - '0' - 1;
                DebugPrintln("Board: continue. {}: {}", i, c);
                continue;
            }
            
            if (c == '/') {
                rank++;
                DebugPrintln("Board: break. {}: {}", i, c);
                break;
            }

            Enums::Colour colour;
            if (isupper(c)) {
                colour = Enums::Colour::White;
            } else {
                colour = Enums::Colour::Black;
            }
            c = tolower(c);

            switch(c) {
            case 'b':
                m_pieces[i] = Piece(colour, Enums::Type::Bishop, i);
                break;
            case 'k':
                m_pieces[i] = Piece(colour, Enums::Type::King,   i);
                break;
            case 'n':
                m_pieces[i] = Piece(colour, Enums::Type::Knight, i);
                break;
            case 'q':
                m_pieces[i] = Piece(colour, Enums::Type::Queen,  i);
                break;
            case 'p':
                m_pieces[i] = Piece(colour, Enums::Type::Pawn,   i);
                break;
            case 'r':
                m_pieces[i] = Piece(colour, Enums::Type::Rook,   i);
                break;
            default:
                m_pieces[i] = Piece();
                break;
            }
        }
    }
}

Board::~Board()
{
    // Nothing todo
}



// ----- Read -----

std::string_view Board::Fen() const noexcept
{
    return m_fen;
}

const Piece* Board::Pieces() const noexcept
{
    return m_pieces;
}



// ----- Update -----

bool Board::MakeMove(std::string_view move)
{

    u8 pos = Fen::MoveToIndex(move);
    u64 spots = MoveGen::Generate(*this, m_pieces[pos]);
    if (!spots) {
        return false;
    }
    
    return true;
}

