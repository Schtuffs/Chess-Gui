#include "../MoveGen/MoveGen.h"

#include <print>

#include "MoveGen/Magic.h"

void AddMoves(MoveList& list, Square from, BitBoard targets)
{
    while (targets) {
        list.moves[list.size] = Move::Make(from, targets.PopLSB());
        list.size++;
    }
}

template <Colour us, PieceType type>
void GenerateMoves(const Position& pos, MoveList& list, BitBoard targets)
{
    BitBoard bb       = pos.Pieces(us, type);
    BitBoard occupied = pos.Pieces();

    while (bb) {
        Square from = bb.PopLSB();

        BitBoard b = Magic::GetAttacks<type>(from, occupied);
        b &= targets;

        AddMoves(list, from, b);
    }
}

template <Colour us, GenType type>
void GeneratePawnMoves(const Position& pos, MoveList& list, BitBoard targets)
{
    (void)targets;

    constexpr Direction forward  = (us == WHITE ? NORTH : SOUTH);
    constexpr Direction attLeft  = (Direction)std::abs(forward + WEST);
    constexpr Direction attRight = (Direction)std::abs(forward + EAST);
    constexpr BitBoard  promo    = (us == WHITE ? RANK_7 : RANK_2);

    if constexpr (type != CAPTURES) {
        BitBoard pawns = pos.Pieces(us, PAWN);

        while (pawns) {
            Square   from = pawns.PopLSB();
            BitBoard target;

            if constexpr (forward == NORTH) {
                target |= (BitBoard(from) << (u8)forward);
                target |= (BitBoard(from) << (u8)(forward * 2));
            } else {
                target |= (BitBoard(from) >> (u8)forward);
                target |= (BitBoard(from) >> (u8)(forward * 2));
            }

            AddMoves(list, from, target);
        }
    }

    if constexpr (type == CAPTURES) {
        BitBoard pawns = pos.Pieces(us, PAWN);

        while (pawns) {
            Square   from = pawns.PopLSB();
            BitBoard target;

            if constexpr (forward == NORTH) {
                target |= (BitBoard(from) << (u8)attLeft) & ~FILE_8;
                target |= (BitBoard(from) << (u8)attRight) & ~FILE_1;
            } else {
                target |= (BitBoard(from) >> (u8)attLeft) & ~FILE_8;
                target |= (BitBoard(from) >> (u8)attRight) & ~FILE_1;
            }

            AddMoves(list, from, target);
        }
    }

    BitBoard occupied = pos.Pieces();
}

template <Colour us, GenType type>
void GenerateAll(const Position& pos, MoveList& list)
{
    BitBoard targets;

    if constexpr (type == CAPTURES) {
        targets = pos.Pieces(~us);
    } else if constexpr (type == QUIETS) {
        targets = ~pos.Pieces();
    }

    GenerateMoves<us, BISHOP>(pos, list, targets);
    GenerateMoves<us, KNIGHT>(pos, list, targets);
    GenerateMoves<us, QUEEN>(pos, list, targets);
    GenerateMoves<us, ROOK>(pos, list, targets);
    GeneratePawnMoves<us, type>(pos, list, targets);
}

template <GenType type>
void MoveGen::Generate(const Position& pos, MoveList& list)
{
    (void)list;
    Colour us = pos.Player();
    (us == WHITE ? GenerateAll<WHITE, type>(pos, list) : GenerateAll<BLACK, type>(pos, list));
}

template void MoveGen::Generate<CAPTURES>(const Position& pos, MoveList& list);
template void MoveGen::Generate<QUIETS>(const Position& pos, MoveList& list);

// ----- Move List -----

void MoveList::Legalize(const Position& pos)
{
    MoveList legal;
    for (u8 i = 0; i < this->size; i++) {
        Move move = this->moves[i];
        if (pos.IsLegal(move)) {
            legal.moves[legal.size] = move;
            legal.size++;
        }
    }
    *this = std::move(legal);
}

Move*       MoveList::begin() noexcept { return this->moves; }
Move*       MoveList::end() noexcept { return (this->moves + this->size); }
const Move* MoveList::begin() const noexcept { return this->moves; }
const Move* MoveList::end() const noexcept { return (this->moves + this->size); }
