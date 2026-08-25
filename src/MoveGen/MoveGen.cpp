#include "../MoveGen/MoveGen.h"

#include <print>

#include "MoveGen/Magic.h"
#include "Utils/Constants.h"

template <Direction dir>
constexpr BitBoard Shift(BitBoard bb)
{
    if constexpr (dir == NORTH) {
        return (bb << u8(dir));
    } else if constexpr (dir == EAST) {
        return (bb << u8(dir));
    } else if constexpr (dir == SOUTH) {
        return (bb >> u8(dir));
    } else if constexpr (dir == WEST) {
        return (bb >> u8(dir));
    } else if constexpr (dir == NORTH_EAST) {
        return (bb << u8(dir));
    } else if constexpr (dir == NORTH_WEST) {
        return (bb << u8(dir));
    } else if constexpr (dir == SOUTH_EAST) {
        return (bb >> u8(dir));
    } else if constexpr (dir == SOUTH_WEST) {
        return (bb >> u8(dir));
    }

    return BitBoard(0);
}

void AddMoves(MoveList& list, Square from, BitBoard targets)
{
    while (targets) {
        list.Add(Move::Make(from, targets.PopLSB()));
    }
}

template <Direction offset>
void AddPawnMoves(MoveList& list, BitBoard targets)
{
    while (targets) {
        Square to = targets.PopLSB();
        list.Add(Move::Make(Square((i8)to - (i8)offset), to));
    }
}

template <GenType type, Direction dir, bool enemy>
void AddPromotions(MoveList& list, Square to)
{
    Square from = Square(to - (Square)dir);

    if constexpr (type == CAPTURES) {
        list.Add(Move::MakePromo(from, to, QUEEN));
    }
    if constexpr ((type == CAPTURES && enemy) || (type == QUIETS && !enemy)) {
        list.Add(Move::MakePromo(from, to, ROOK));
        list.Add(Move::MakePromo(from, to, BISHOP));
        list.Add(Move::MakePromo(from, to, KNIGHT));
    }
}

template <Colour us, PieceType type>
void GenerateMoves(const Position& pos, MoveList& list, BitBoard valid)
{
    BitBoard pieces   = pos.Pieces(us, type);
    BitBoard occupied = pos.Pieces();

    while (pieces) {
        Square   from = pieces.PopLSB();
        BitBoard bb   = Magic::GetAttacks<type>(from, occupied) & valid;
        AddMoves(list, from, bb);
    }
}

template <Colour us, GenType type>
void GeneratePawnMoves(const Position& pos, MoveList& list, BitBoard valid)
{
    (void)valid;
    constexpr BitBoard  rank3   = (us == WHITE ? RANK_3BB : RANK_6BB);
    constexpr BitBoard  rank7   = (us == WHITE ? RANK_7BB : RANK_2BB);
    constexpr Direction up      = (us == WHITE ? NORTH : SOUTH);
    constexpr Direction upRight = (us == WHITE ? NORTH_EAST : SOUTH_WEST);
    constexpr Direction upLeft  = (us == WHITE ? NORTH_WEST : SOUTH_EAST);

    const BitBoard empty   = ~pos.Pieces();
    const BitBoard enemies = pos.Pieces(~us);

    BitBoard promoting    = pos.Pieces(us, PAWN) & rank7;
    BitBoard notPromoting = pos.Pieces(us, PAWN) & (~rank7);

    // Forward movement
    if constexpr (type != CAPTURES) {
        BitBoard b1 = Shift<up>(notPromoting) & empty;
        BitBoard b2 = Shift<up>(b1 & rank3) & empty;

        AddPawnMoves<up>(list, b1);
        AddPawnMoves<up + up>(list, b2);
    }

    // Promotions
    if (promoting) {
        BitBoard b1 = Shift<upRight>(promoting) & enemies;
        BitBoard b2 = Shift<upLeft>(promoting) & enemies;
        BitBoard b3 = Shift<up>(promoting) & empty;

        while (b1) {
            AddPromotions<type, upRight, true>(list, b1.PopLSB());
        }

        while (b2) {
            AddPromotions<type, upLeft, true>(list, b2.PopLSB());
        }

        while (b3) {
            AddPromotions<type, up, false>(list, b3.PopLSB());
        }
    }

    // Captures
    if constexpr (type == CAPTURES) {
        BitBoard b1 = Shift<upRight>(notPromoting) & enemies;
        BitBoard b2 = Shift<upLeft>(notPromoting) & enemies;

        AddPawnMoves<upRight>(list, b1);
        AddPawnMoves<upLeft>(list, b2);

        if (pos.EnPassant() != SQ_BAD) {

            b1 = notPromoting &
                 (BitBoard(pos.EnPassant() + upRight) | BitBoard(pos.EnPassant() + upLeft));

            while (b1) {
                list.Add(Move::MakeEnPassant(b1.PopLSB(), pos.EnPassant()));
            }
        }
    }
}

template <Colour us, GenType type>
void GenerateAll(const Position& pos, MoveList& list)
{
    BitBoard valid;

    if (pos.Checkers() != 2) {
        if constexpr (type == CAPTURES) {
            valid = pos.Pieces(~us);
        } else if constexpr (type == QUIETS) {
            valid = ~pos.Pieces();
        }

        GenerateMoves<us, BISHOP>(pos, list, valid);
        GenerateMoves<us, KNIGHT>(pos, list, valid);
        GenerateMoves<us, QUEEN>(pos, list, valid);
        GenerateMoves<us, ROOK>(pos, list, valid);
        GeneratePawnMoves<us, type>(pos, list, valid);
    }

    Square   ksq = pos.Pieces(us, KING).Sq();
    BitBoard bb  = Magic::GetAttacks<KING>(ksq, pos.Pieces()) & valid;
    AddMoves(list, ksq, bb);

    if constexpr (type == QUIETS) {
        // Kingside
        Square target = Square(ksq + 2);
        if (pos.IsCastleLegal(ksq, target)) {
            list.Add(Move::MakeCastle(ksq, target));
        }
        
        // Queenside
        target = Square(ksq - 2);
        if (pos.IsCastleLegal(ksq, target)) {
            list.Add(Move::MakeCastle(ksq, target));
        }
    }
}

template <GenType type>
void MoveGen::Generate(const Position& pos, MoveList& list)
{
    Colour us = pos.Player();
    (us == WHITE ? GenerateAll<WHITE, type>(pos, list) : GenerateAll<BLACK, type>(pos, list));
}

template void MoveGen::Generate<CAPTURES>(const Position& pos, MoveList& list);
template void MoveGen::Generate<QUIETS>(const Position& pos, MoveList& list);

void MoveGen::Generate(const Position& pos, MoveList& list)
{
    MoveGen::Generate<CAPTURES>(pos, list);
    MoveGen::Generate<QUIETS>(pos, list);
}

// ----- Move List -----

void MoveList::Legalize(Position& pos)
{
    MoveList legal;
    for (u8 i = 0; i < this->size; i++) {
        Move move = this->moves[i];
        // pos.MakeMove(move);
        if (pos.IsLegal(move)) {
            legal.Add(move);
        }
        // pos.UnmakeMove(move);
    }
    *this = std::move(legal);
}

BitBoard MoveList::ToBB(Square from) const noexcept
{
    BitBoard bb;
    for (Move move : moves) {
        if (move.From() == from) {
            bb |= move.To();
        }
    }
    return bb;
}

Move*       MoveList::begin() noexcept { return this->moves; }
Move*       MoveList::end() noexcept { return (this->moves + this->size); }
const Move* MoveList::begin() const noexcept { return this->moves; }
const Move* MoveList::end() const noexcept { return (this->moves + this->size); }
