#include <stdio.h>
#include "defs.h"

long leafnodes;

void Perft(int depth, S_BOARD *pos)
{
    ASSERT(CheckBoard(pos));

    if(depth == 0)
    {
        leafnodes++;
        return;
    }

    S_MOVELIST list[1];
    GenerateAllMoves(pos, list);
    
    for(int moveNum=0; moveNum<list->count; moveNum++)
    {
        if(!MakeMove(pos, list->moves[moveNum].move)) continue;
        Perft(depth-1, pos);
        TakeMove(pos);
    }

    return;
}

void PerftTest(int depth, S_BOARD *pos) 
{

    ASSERT(CheckBoard(pos));

	PrintBoard(pos);
	printf("\nStarting Test To Depth:%d\n",depth);	
	leafnodes = 0;
	int start = GetTimeMs();
    S_MOVELIST list[1];
    GenerateAllMoves(pos,list);	
    
    int move;	    
    int MoveNum = 0;
	for(MoveNum = 0; MoveNum < list->count; ++MoveNum) 
    {
        move = list->moves[MoveNum].move;
        if ( !MakeMove(pos,move)) continue;

        long cumnodes = leafnodes;
        Perft(depth - 1, pos);
        TakeMove(pos);        
        long oldnodes = leafnodes - cumnodes;
        printf("move %d : %s : %ld\n",MoveNum+1,PrMove(move),oldnodes);
    }
	
	printf("\nTest Complete : %ld nodes visited in %dms\n",leafnodes,GetTimeMs() - start);
    // printf("\nTest Complete : %ld nodes visited\n",leafnodes);

    return;
}