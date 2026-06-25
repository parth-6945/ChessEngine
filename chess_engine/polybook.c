#include <stdio.h>
#include "defs.h"
#include "polykeys.h"

typedef struct
{
	U64 key;
	unsigned short move;
	unsigned short weight;
	unsigned int learn;
	
} S_POLY_BOOK_ENTRY;

long NumEntries = 0;

S_POLY_BOOK_ENTRY *entries;

const int PolyKindOfPiece[13] = {-1, 1, 3, 5, 7, 9, 11, 0, 2, 4, 6, 8, 10};

void InitPolyBook()
{
    EngineOptions->UseBook = FALSE;
    FILE *pFile = fopen("performance.bin", "rb");
    entries = NULL;
    
    if(pFile == NULL)
    {
        printf("Book file not read\n");
    }
    else
    {
        fseek(pFile, 0, SEEK_END);
        long position = ftell(pFile);

        if(position < sizeof(S_POLY_BOOK_ENTRY))
        {
            printf("No valid entries found\n");
            return;
        }

        NumEntries = position / sizeof(S_POLY_BOOK_ENTRY);
        printf("Num of entries found in file : %ld\n", NumEntries);

        entries = (S_POLY_BOOK_ENTRY*)malloc(NumEntries*sizeof(S_POLY_BOOK_ENTRY));
        rewind(pFile);

        size_t returnVal;
        returnVal = fread(entries, sizeof(S_POLY_BOOK_ENTRY), NumEntries, pFile);
        printf("fread() %ld Entries Read in from file\n", returnVal);

        if(NumEntries > 0) EngineOptions->UseBook = TRUE;
    }
}

void CleanPolyBook()
{
    if(entries == NULL) return;
    free(entries);
}

int HasPawnForCapture(const S_BOARD *pos)
{
    int sqWithPawn = 0;
    int targetPiece = (pos->side == WHITE)? wP : wB;

    if(pos->enPas != NO_SQ)
    {
        if(pos->side == WHITE)  sqWithPawn = pos->enPas -10;
        else                    sqWithPawn = pos->enPas +10;

        if(pos->pieces[sqWithPawn+1] == targetPiece) return TRUE;
        else if(pos->pieces[sqWithPawn-1] == targetPiece) return TRUE;
    }
    return FALSE;
}

U64 PolyKeyFromBoard(S_BOARD *pos)
{
    int sq = 0, rank = 0, file = 0;
    U64 finalKey = 0;
    int piece = EMPTY;
    int polyPiece = 0;
    int offset = 0;

    for(sq = 0; sq < BRD_SQ_NUM; ++sq)
    {
		piece = pos->pieces[sq];
		if(piece != NO_SQ && piece != EMPTY && piece != OFFBOARD)
        {
			ASSERT(piece >= wP && piece <= bK);
			polyPiece = PolyKindOfPiece[piece];
			rank = RanksBrd[sq];
			file = FilesBrd[sq];
			finalKey ^= Random64Poly[(64 * polyPiece) + (8 * rank) + file];
		}
	}

    // castling
    offset = 768;

    if(pos->castlePerm & WKCA) finalKey ^= Random64Poly[offset + 0];
	if(pos->castlePerm & WQCA) finalKey ^= Random64Poly[offset + 1];
	if(pos->castlePerm & BKCA) finalKey ^= Random64Poly[offset + 2];
	if(pos->castlePerm & BQCA) finalKey ^= Random64Poly[offset + 3];

    // enpassant
	offset = 772;
	if(HasPawnForCapture(pos) == TRUE)
    {
		file = FilesBrd[pos->enPas];
		finalKey ^= Random64Poly[offset + file];
	}
	
	if(pos->side == WHITE) finalKey ^= Random64Poly[780];

    return finalKey;
}

unsigned short endian_swap_u16(unsigned short x) 
{ 
    x = (x>>8) | 
        (x<<8); 
    return x;
} 

unsigned int endian_swap_u32(unsigned int x) 
{ 
    x = (x>>24) | 
        ((x<<8) & 0x00FF0000) | 
        ((x>>8) & 0x0000FF00) | 
        (x<<24); 
    return x;
} 

U64 endian_swap_u64(U64 x) 
{ 
    x = (x>>56) | 
        ((x<<40) & 0x00FF000000000000) | 
        ((x<<24) & 0x0000FF0000000000) | 
        ((x<<8)  & 0x000000FF00000000) | 
        ((x>>8)  & 0x00000000FF000000) | 
        ((x>>24) & 0x0000000000FF0000) | 
        ((x>>40) & 0x000000000000FF00) | 
        (x<<56); 
    return x;
}

int ConvertPolyMoveToInternalMove(unsigned short polyMove, S_BOARD *pos)
{
	
	int ff = (polyMove >> 6) & 7;
	int fr = (polyMove >> 9) & 7;
	int tf = (polyMove >> 0) & 7;
	int tr = (polyMove >> 3) & 7;
	int pp = (polyMove >> 12) & 7;
	
	char moveString[6];
	if(pp == 0)
    {
		sprintf(moveString, "%c%c%c%c",
		FileChar[ff],
		RankChar[fr],
		FileChar[tf],
		RankChar[tr]);
	}
    else
    {
		char promChar = 'q';
		switch(pp)
        {
			case 1: promChar = 'n'; break;
			case 2: promChar = 'b'; break;
			case 3: promChar = 'r'; break;
		}
		sprintf(moveString, "%c%c%c%c%c",
            FileChar[ff],
            RankChar[fr],
            FileChar[tf],
            RankChar[tr],
            promChar);
	}
	
	return ParseMove(moveString, pos);
}

int GetBookMove(S_BOARD *pos)
{
    int index = 0;
    S_POLY_BOOK_ENTRY *entry;
    unsigned short move;
    const int MAXBOOKMOVES = 32;
    int bookMoves[MAXBOOKMOVES];
    int tempMove = NOMOVE;
    int count = 0;

    U64 polyKey = PolyKeyFromBoard(pos);

    for(entry = entries; entry < entries + NumEntries; entry++)
    {
        if(polyKey == endian_swap_u64(entry->key))
        {
            move = endian_swap_u16(entry->move);
            tempMove = ConvertPolyMoveToInternalMove(move, pos);
            if(tempMove != NOMOVE)
            {
				bookMoves[count++] = tempMove;
				if(count > MAXBOOKMOVES) break;
			}
        }
    }

    if(count != 0)
    {
		int randMove = rand() % count;
		return bookMoves[randMove];
	}
    else return NOMOVE;
}