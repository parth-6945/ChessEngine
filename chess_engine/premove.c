#include <stdio.h>
#include "defs.h"
#include <string.h>

#define FEN1  "rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1"
#define FEN2  "rnbqkbnr/pp1ppppp/8/2p5/4P3/8/PPPP1PPP/RNBQKBNR w KQkq c6 0 2"
#define FEN3  "rnbqkbnr/pp1ppppp/8/2p5/4P3/5N2/PPPP1PPP/RNBQKB1R b KQkq - 1 2"
#define FEN4  "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1"
#define FEN5  "8/3n4/2b5/8/4Q3/8/8/8 w - - 0 2"
#define FEN6  "rnbqkb1r/pp1p1pPp/8/2p1pP2/1P1P4/3P3P/P1P1P3/RNBQKBNR w KQkq e6 0 1"
#define FEN7  "rnbqkbnr/p1p1p3/3p3p/1p1p4/2P1Pp2/8/PP1P1PpP/RNBQKB1R b KQkq e3 0 1"
#define FEN8  "5k2/1n6/4n3/6N1/8/3N4/8/5K2 w - - 0 1"
#define FEN9  "6k1/8/5r2/8/1nR5/5N2/8/6K1 b - - 0 1"
#define FEN10 "6k1/8/4nq2/8/1nQ5/5N2/1N6/6K1 b - - 0 1"
#define FEN11 "6k1/1b6/4n3/8/1n4B1/1B3N2/1N6/2b3K1 b1/8 - - 0 1"
#define FEN12 "r3k2r/8/8/8/8/8/8/R3K2R w KQkq - 0 1"
#define FEN13 "3rk2r/8/8/8/8/8/6p1/R3K2R b KQk - 0 1"
#define FEN14 "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1"
#define FEN15 "2rr3k/pp3pp1/1nnqbN1p/3pN3/2pP4/2P3Q1/PPB4P/R4RK1 w - -"
#define FEN16 "r1b1k2r/ppppnppp/2n2q2/2b5/3NP3/2P1B3/PP3PPP/RN1QKB1R w KQkq - 0 1"

// void ShowSqAtBySide(const int side, const S_BOARD *pos)
// {
//     int sq = 0;
//     int rank = 0;
//     int file = 0;

//     printf("Squares attacked by %c : \n\n", SideChar[side]);

//     for(rank = RANK_8; rank >= RANK_1; --rank)
//     {
//         for(file = FILE_A; file <= FILE_H; file++)
//         {
//             sq = FR2SQ(file, rank);
//             if (SqAttacked(sq, side, pos) == TRUE)
//             {
//                 printf(" X ");
//             }
//             else printf(" - ");            
//         }
//         printf("\n");
//     }

//     printf("\n");
// }

int main(int argc, char *argv[])
{
    AllInit();

    S_BOARD board[1];
    S_SEARCHINFO info[1];
	info->quit = FALSE;
	board->HashTable->pTable = NULL;
	InitHashTable(board->HashTable, 64);

	int ArgNum = 0;
    
    for(ArgNum = 0; ArgNum < argc; ++ArgNum)
	{
    	if(strncmp(argv[ArgNum], "NoBook", 6) == 0)
		{
    		EngineOptions->UseBook = FALSE;
    		printf("Book Off\n");
    	}
    }

    printf("Welcome to Premove! Type 'premove' for console mode...\n");

	char line[256];
	while (TRUE) {
		memset(&line[0], 0, sizeof(line));

		fflush(stdout);
		if (!fgets(line, 256, stdin))
			continue;
		if (line[0] == '\n')
			continue;
		if (!strncmp(line, "uci",3)) {
			Uci_Loop(board, info);
			if(info->quit == TRUE) break;
			continue;
		} else if (!strncmp(line, "xboard",6))	{
			XBoard_Loop(board, info);
			if(info->quit == TRUE) break;
			continue;
		} else if (!strncmp(line, "premove",7))	{
			Console_Loop(board, info);
			if(info->quit == TRUE) break;
			continue;
		} else if(!strncmp(line, "quit",4))	{
			break;
		}
	}

	free(board->HashTable->pTable);
	CleanPolyBook();

    return 0;
}