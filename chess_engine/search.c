#include <stdio.h>
#include "defs.h"

static void CheckUp(S_SEARCHINFO *info)
{
    // Checks if time up or interrupt from gui
    if(info->timeset == TRUE && GetTimeMs() > info->stoptime) info->stopped = TRUE;

    ReadInput(info);
}

static void PickNextMove(int moveNum, S_MOVELIST *list)
{
    S_MOVE temp;
    int index = 0;
    int bestScore = 0;
    int bestNum = moveNum;

    for(index=moveNum; index < list->count; index++)
    {
        if(list->moves[index].score > bestScore)
        {
            bestScore = list->moves[index].score;
            bestNum = index;
        }
    }
    temp = list->moves[moveNum];
    list->moves[moveNum] = list->moves[bestNum];
    list->moves[bestNum] = temp;
}

static int IsRepeatition(const S_BOARD *pos)
{
    int index = pos->hisPly - pos->fiftyMove;

    for(; index < pos->hisPly-1; index++)
    {
        ASSERT(index >= 0 && index < MAXGAMEMOVES);
        if(pos->posKey == pos->history[index].posKey) return TRUE;
    }

    return FALSE;
}

static void ClearForSearch(S_BOARD* pos, S_SEARCHINFO *info)
{
    int index = 0;
    int index2 = 0;

    for(index=0; index < 13; index++)
    {
        for(index2=0; index2 < BRD_SQ_NUM; index2++)
        {
            pos->searchHistory[index][index2] = 0;
        }
    }
    
    for(index=0; index < 2; index++)
    {
        for(index2=0; index2 < MAXDEPTH; index2++)
        {
            pos->searchKillers[index][index2] = 0;
        }
    }

    ClearHashTable(pos->HashTable);
    pos->ply = 0;

    info->stopped   = 0;
    info->nodes     = 0;
    info->fh        = 0;
    info->fhf       = 0;
}

static int Quiescence(int alpha, int beta, S_BOARD* pos, S_SEARCHINFO *info)
{
    ASSERT(CheckBoard(pos));

    if((info->nodes & 2047) == 0)
    {
        CheckUp(info);
    }

    info->nodes++;

    if(IsRepeatition(pos) || pos->fiftyMove >= 100) return 0;
    if(pos->ply > MAXDEPTH-1) return EvaluatePosition(pos);

    int Score = EvaluatePosition(pos);

    if(Score >= beta)
    {
        return beta;
    }
    if(Score > alpha)
    {
        alpha = Score;
    }

    S_MOVELIST list[1];
    GenerateAllCaps(pos, list);

    int MoveNum = 0;
    int Legal = 0;
    int OldAlpha = alpha;
    int BestMove = NOMOVE;
    Score = -INF_BOUND;
    int PvMove = ProbePvMove(pos);

    for(MoveNum=0; MoveNum < list->count; MoveNum++)
    {

        PickNextMove(MoveNum, list);

        if(!MakeMove(pos, list->moves[MoveNum].move)) continue;

        Legal++;
        Score = -Quiescence(-beta, -alpha, pos, info);
        TakeMove(pos);

        if(info->stopped == TRUE) return 0;

        if(Score > alpha)
        {
            if(Score >= beta)
            {
                if(Legal == 1) info->fhf++;
                info->fh++;
                return beta;
            }
            alpha = Score;
            BestMove = list->moves[MoveNum].move;
        }
    }

    /* Quiescence does not store into the main hash table */

    return alpha;
}

static int AlphaBeta(int alpha, int beta, int depth, S_BOARD* pos, S_SEARCHINFO *info, int DoNull)
{
    ASSERT(CheckBoard(pos));

    if(depth <= 0)
    {
        // info->nodes++;
        // return EvaluatePosition(pos);
        return Quiescence(alpha, beta, pos, info);
    }

    if((info->nodes & 2047) == 0)
    {
        CheckUp(info);
    }

    info->nodes++;

    if(IsRepeatition(pos) || pos->fiftyMove >= 100 && pos->ply) return 0;

    if(pos->ply > MAXDEPTH-1) return EvaluatePosition(pos);

    int InCheck = SqAttacked(pos->kingSq[pos->side], pos->side^1, pos);
    if (InCheck) depth++;

    int Score = -INF_BOUND;

    if( DoNull && !InCheck && pos->ply && (pos->bigPce[pos->side] > 1) && depth >= 4)
    {
		MakeNullMove(pos);
		Score = -AlphaBeta( -beta, -beta + 1, depth-4, pos, info, FALSE);
		TakeNullMove(pos);
		if(info->stopped == TRUE) return 0;

		if (Score >= beta)
        {
			info->nullCut++;
			return beta;
		}
	}

    S_MOVELIST list[1];
    int PvMove = NOMOVE;
    Score = -INF_BOUND;

    if( ProbeHashEntry(pos, &PvMove, &Score, alpha, beta, depth) == TRUE ) {
        pos->HashTable->cut++;
        return Score;
    }

    GenerateAllMoves(pos, list);

    int MoveNum = 0;
    int Legal = 0;
    int OldAlpha = alpha;
    int BestMove = NOMOVE;
    Score = -INF_BOUND;
    int BestScore = -INF_BOUND;

    if(PvMove != NOMOVE)
    {
        for(MoveNum=0; MoveNum < list->count; MoveNum++)
        {
            if(list->moves[MoveNum].move == PvMove)
            {
                list->moves[MoveNum].score = 2000000;
                break;
            }
        }
    }

    for(MoveNum=0; MoveNum < list->count; MoveNum++)
    {

        PickNextMove(MoveNum, list);

        if(!MakeMove(pos, list->moves[MoveNum].move)) continue;

        Legal++;
        Score = -AlphaBeta(-beta, -alpha, depth-1, pos, info, TRUE);
        TakeMove(pos);

        if(info->stopped == TRUE) return 0;

        if(Score > BestScore)
        {
            BestScore = Score;
            BestMove = list->moves[MoveNum].move;
            if(Score > alpha)
            {
                if(Score >= beta)
                {
                    if(Legal == 1) info->fhf++;
                    info->fh++;

                    if(!(list->moves[MoveNum].move & MFLAGCAP))
                    {
                        pos->searchKillers[1][pos->ply] = pos->searchKillers[0][pos->ply];
                        pos->searchKillers[0][pos->ply] = list->moves[MoveNum].move;
                    }

                    StoreHashEntry(pos, list->moves[MoveNum].move, beta, HFBETA, depth);

                    return beta;
                }
                alpha = Score;
                BestMove = list->moves[MoveNum].move;
                if(!(list->moves[MoveNum].move & MFLAGCAP))
                {
                    pos->searchHistory[pos->pieces[FROMSQ(BestMove)]][TOSQ(BestMove)] += depth;
                }
            }
        }
    }

    if(Legal == 0)
    {
        if(InCheck)
            return -INF_BOUND + pos->ply;
        else return 0;
    }

    if(alpha != OldAlpha)
    {
        StoreHashEntry(pos, BestMove, BestScore, HFEXACT, depth);
    } else {
        StoreHashEntry(pos, BestMove, alpha, HFALPHA, depth);
    }

    return alpha;
}

void SearchPosition(S_BOARD* pos, S_SEARCHINFO *info)
{
    int bestMove     = NOMOVE;
    int bestScore    = -INF_BOUND;
    int currentDepth = 0;
    int pvMoves      = 0;
    int pvNum        = 0;

    ClearForSearch(pos, info);

    // Book move usage
    if(EngineOptions->UseBook == TRUE)
    {
        bestMove = GetBookMove(pos);
    }

    // Iterative deepening and search init
    if(bestMove == NOMOVE)
    {
        for(currentDepth = 1; currentDepth <= info->depth; currentDepth++)
        {
            bestScore = AlphaBeta(-INF_BOUND, INF_BOUND, currentDepth, pos, info, TRUE);

            if(info->stopped == TRUE) break;

            pvMoves = GetPvLine(currentDepth, pos);
            bestMove = pos->PvArray[0];

            if(info->GAME_MODE == UCIMODE) {
                printf("info score cp %d depth %d nodes %ld time %d ",
                    bestScore,currentDepth,info->nodes,GetTimeMs()-info->starttime);
            } else if(info->GAME_MODE == XBOARDMODE && info->POST_THINKING == TRUE) {
                printf("%d %d %d %ld ",
                    currentDepth,bestScore,(GetTimeMs()-info->starttime)/10,info->nodes);
            } else if(info->POST_THINKING == TRUE) {
                printf("score:%d depth:%d nodes:%ld time:%d(ms) ",
                    bestScore,currentDepth,info->nodes,GetTimeMs()-info->starttime);
            }
            if(info->GAME_MODE == UCIMODE || info->POST_THINKING == TRUE) {
                pvMoves = GetPvLine(currentDepth, pos);
                if(!info->GAME_MODE == XBOARDMODE) {
                    printf("pv");
                }
                for(pvNum = 0; pvNum < pvMoves; ++pvNum) {
                    printf(" %s",PrMove(pos->PvArray[pvNum]));
                }
                printf("\n");
            }
        }
    }
    if(info->GAME_MODE == UCIMODE) {
		printf("bestmove %s\n",PrMove(bestMove));
	} else if(info->GAME_MODE == XBOARDMODE) {
		printf("move %s\n",PrMove(bestMove));
		MakeMove(pos, bestMove);
	} else {
		printf("\n\n***!! Premove makes move %s !!***\n\n",PrMove(bestMove));
		MakeMove(pos, bestMove);
		PrintBoard(pos);
	}
}