#include "Board.h"



// ----- Creation ----- Destruction -----

Board::Board(std::string_view fen)
    : m_fen(fen)
{}

Board::~Board()
{
    // Nothing todo
}



// ----- Read -----

std::string_view Board::Fen()
{
    return m_fen;
}



// ----- Update -----

bool Board::MakeMove(std::string_view move)
{
    (void)move;
    
    return true;
}

