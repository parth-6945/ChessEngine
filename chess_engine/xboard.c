#include "stdio.h"
#include "defs.h"
#include "string.h"

int ThreeFoldRep(const S_BOARD *pos) 
{

	ASSERT(CheckBoard(pos));

	int i = 0, r = 0;
	for (i = 0; i < pos->hisPly; ++i)
    {
	    if (pos->history[i].posKey == pos->posKey)
        {
		    r++;
		}
	}
	return r;
}

int DrawMaterial(const S_BOARD *pos) 
{
	ASSERT(CheckBoard(pos));

    if (pos->pceNum[wP] || pos->pceNum[bP]) return FALSE;
    if (pos->pceNum[wQ] || pos->pceNum[bQ] || pos->pceNum[wR] || pos->pceNum[bR]) return FALSE;
    if (pos->pceNum[wB] > 1 || pos->pceNum[bB] > 1) {return FALSE;}
    if (pos->pceNum[wN] > 1 || pos->pceNum[bN] > 1) {return FALSE;}
    if (pos->pceNum[wN] && pos->pceNum[wB]) {return FALSE;}
    if (pos->pceNum[bN] && pos->pceNum[bB]) {return FALSE;}

    return TRUE;
}

int checkresult(S_BOARD *pos)
{
	ASSERT(CheckBoard(pos));

    if (pos->fiftyMove > 100) {
     printf("1/2-1/2 {fifty move rule (claimed by Vice)}\n"); return TRUE;
    }

    if (ThreeFoldRep(pos) >= 2) {
     printf("1/2-1/2 {3-fold repetition (claimed by Vice)}\n"); return TRUE;
    }

	if (DrawMaterial(pos) == TRUE) {
     printf("1/2-1/2 {insufficient material (claimed by Vice)}\n"); return TRUE;
    }

    S_MOVELIST list[1];
    GenerateAllMoves(pos,list);

    int MoveNum = 0;
	int found = 0;
	for(MoveNum = 0; MoveNum < list->count; ++MoveNum) {

        if ( !MakeMove(pos,list->moves[MoveNum].move))  {
            continue;
        }
        found++;
		TakeMove(pos);
		break;
    }

    if(found != 0) return FALSE;

	int InCheck = SqAttacked(pos->kingSq[pos->side],pos->side^1,pos);

	if(InCheck == TRUE)	{
	    if(pos->side == WHITE) {
	      printf("0-1 {black mates (claimed by Vice)}\n");return TRUE;
        } else {
	      printf("0-1 {white mates (claimed by Vice)}\n");return TRUE;
        }
    } else {
      printf("\n1/2-1/2 {stalemate (claimed by Vice)}\n");return TRUE;
    }
	return FALSE;
}

void XBoard_Loop(S_BOARD *pos, S_SEARCHINFO *info)
{
    setbuf(stdin, NULL);
    setbuf(stdout, NULL);

    int depth = -1, movestogo[2] = {30,30 }, movetime = -1;
	int time = -1, inc = 0;
	int engineSide = BOTH;
	int timeLeft;
	int sec;
	int mps;
	int move = NOMOVE;
    int i, score;
	char inBuf[80], command[80];

    while (TRUE)
    {
        fflush(stdout);

        if(pos->side == engineSide)
        {

        }

        fflush(stdout);

        memset(&inBuf[0], 0, sizeof(inBuf));
		fflush(stdout);
		if (!fgets(inBuf, 80, stdin))
		continue;

        sscanf(inBuf, "%s", command);

        
    }
    
}