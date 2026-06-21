#include <stdio.h>
#include "defs.h"

int CountBits(U64 bb) 
{
    int count = 0;
    while (bb) 
    {
        bb &= (bb - 1); // Clear the least significant bit set
        count++;
    }
    return count;
}

int PopBit(U64 *bb) 
{
    U64 b = *bb ^ (*bb - 1);
    int count = CountBits(b - 1);
    *bb &= (*bb - 1); // Clear the least significant bit set
    return count;
}

void PrintBitBoard(U64 bb) 
{
    printf("\n");

    for (int rank = RANK_8; rank >= RANK_1; rank--) 
    {
        for (int file = FILE_A; file <= FILE_H; file++) 
        {
            int sq   = FR2SQ(file, rank);
            int sq64 = SQ64(sq);

            printf("%c", ((bb & (1ULL << sq64)) ? 'X' : '-'));
        }
        printf("\n");
    }
    printf("\n");
}