#include "defs.h"
#include <stdio.h>
#include <stdlib.h>

#define RAND_64 ((U64)rand() | \
                  ((U64)rand() << 15) | \
                  ((U64)rand() << 30) | \
                  ((U64)rand() << 45) | \
                  (((U64)rand() & 0xf) << 60))

int Sq120ToSq64[BRD_SQ_NUM];
int Sq64ToSq120[64];

U64 PieceKeys[13][120];
U64 SideKey;
U64 CastleKeys[16];

int FilesBrd[BRD_SQ_NUM];
int RanksBrd[BRD_SQ_NUM];

U64 FileBBMask[8];
U64 RankBBMask[8];

U64 BlackPassedMask[64];
U64 WhitePassedMask[64];
U64 IsolatedMask[64];

S_OPTIONS EngineOptions[1];

void InitEvalMask()
{
    int sq, tsq, r, f;

    for(sq = 0; sq < 8; ++sq) {
        FileBBMask[sq] = 0ULL;
		RankBBMask[sq] = 0ULL;
	}

	for(r = 0; r < 8; r++)
    {
        FileBBMask[r] = (0x0101010101010101) << r;
        RankBBMask[r] = (0x00000000000000ff) << (r<<3);
	}

    for(sq = 0; sq < 64; ++sq)
    {
		IsolatedMask[sq] = 0ULL;
		WhitePassedMask[sq] = 0ULL;
		BlackPassedMask[sq] = 0ULL;
    }

    for(sq = 0; sq < 64; ++sq) {
		tsq = sq + 8;

        while(tsq < 64) {
            WhitePassedMask[sq] |= (1ULL << tsq);
            tsq += 8;
        }

        tsq = sq - 8;
        while(tsq >= 0) {
            BlackPassedMask[sq] |= (1ULL << tsq);
            tsq -= 8;
        }

        if(FilesBrd[SQ120(sq)] > FILE_A) {
            IsolatedMask[sq] |= FileBBMask[FilesBrd[SQ120(sq)] - 1];

            tsq = sq + 7;
            while(tsq < 64) {
                WhitePassedMask[sq] |= (1ULL << tsq);
                tsq += 8;
            }

            tsq = sq - 9;
            while(tsq >= 0) {
                BlackPassedMask[sq] |= (1ULL << tsq);
                tsq -= 8;
            }
        }

        if(FilesBrd[SQ120(sq)] < FILE_H) {
            IsolatedMask[sq] |= FileBBMask[FilesBrd[SQ120(sq)] + 1];

            tsq = sq + 9;
            while(tsq < 64) {
                WhitePassedMask[sq] |= (1ULL << tsq);
                tsq += 8;
            }

            tsq = sq - 7;
            while(tsq >= 0) {
                BlackPassedMask[sq] |= (1ULL << tsq);
                tsq -= 8;
            }
        }
	}
}

void InitFilesRanksBrd()
{
    for(int index = 0; index < BRD_SQ_NUM; index++)
    {
        FilesBrd[index] = OFFBOARD;
        RanksBrd[index] = OFFBOARD;
    }

    for(int rank = RANK_1; rank <= RANK_8; rank++)
    {
        for(int file = FILE_A; file <= FILE_H; file++)
        {
            int sq = FR2SQ(file, rank);
            FilesBrd[sq] = file;
            RanksBrd[sq] = rank;
        }
    }
}

void InitHashKeys()
{
    for(int piece = 0; piece < 13; piece++)
    {
        for(int sq = 0; sq < BRD_SQ_NUM; sq++)
        {
            PieceKeys[piece][sq] = RAND_64;
        }
    }
    SideKey = RAND_64;
    for(int i = 0; i < 16; i++)
    {
        CastleKeys[i] = RAND_64;
    }
}

void InitSq120To64()
{
    int index = 0;
    for(int i = 0; i < BRD_SQ_NUM; i++)
    {
        Sq120ToSq64[i] = 65;
    }
    for(int i = 0; i < 64; i++)
    {
        Sq64ToSq120[i] = 120;
    }
    for(int rank = RANK_1; rank <= RANK_8; rank++)
    {
        for(int file = FILE_A; file <= FILE_H; file++)
        {
            int sq120 = FR2SQ(file, rank);
            Sq120ToSq64[sq120] = index;
            Sq64ToSq120[index] = sq120;
            index++;
        }
    }
}

void AllInit()
{
    InitSq120To64();
    InitHashKeys();
    InitFilesRanksBrd();
    InitEvalMask();
    InitMvvLva();
    InitPolyBook();
}