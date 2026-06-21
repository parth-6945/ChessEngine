#include <stdio.h>
#include "defs.h"

const int KnDir[8] = { -8, -19, -21, -12, 8, 19, 21, 12 };
const int RkDir[8] = { -1, -10, 1, 10};
const int BiDir[4] = { -9, -11, 11, 9 };
const int KiDir[8] = { -1, -10, 1, 10, -9, -11, 11, 9 };

int SqAttacked(const int sq, const int side, const S_BOARD *pos) 
{
    int pce, t_sq, dir;

    ASSERT(SqOnBoard(sq));
    ASSERT(SideValid(side));
    ASSERT(CheckBoard(pos));

    // Pawns
    if (side == WHITE) 
    {
        if (pos->pieces[sq - 11] == wP || pos->pieces[sq - 9] == wP) return TRUE;
    } 
    else 
    {
        if (pos->pieces[sq + 11] == bP || pos->pieces[sq + 9] == bP) return TRUE;
    }

    // Knights
    for (dir = 0; dir < 8; dir++) 
    {
        pce = pos->pieces[sq + KnDir[dir]];
        if (pce != OFFBOARD && IsKn(pce) && PieceCol[pce] == side) 
        {
            return TRUE;
        }
    }

    // Rooks, Queens
    for (dir = 0; dir < 4; dir++) 
    {
        t_sq = sq + RkDir[dir];
        pce = pos->pieces[t_sq];
        while (pce != OFFBOARD) 
        {
            if (pce != EMPTY) 
            {
                if (IsRQ(pce) && PieceCol[pce] == side) return TRUE;
                break;
            }
            t_sq += RkDir[dir];
            pce = pos->pieces[t_sq];
        }
    }

    // Bishops, Queens
    for (dir = 0; dir < 4; dir++) 
    {
        t_sq = sq + BiDir[dir];
        pce = pos->pieces[t_sq];
        while (pce != OFFBOARD) 
        {
            if (pce != EMPTY) 
            {
                if (IsBQ(pce) && PieceCol[pce] == side) return TRUE;
                break;
            }
            t_sq += BiDir[dir];
            pce = pos->pieces[t_sq];
        }
    }

    // Kings
    for (dir = 0; dir < 8; dir++) 
    {
        pce = pos->pieces[sq + KiDir[dir]];
        if (pce != OFFBOARD && IsKi(pce) && PieceCol[pce] == side) return TRUE;
    }

    return FALSE;
}