#pragma once
#include "stdafx.h"
#include "ABAI.h"
#include "IO.h"
#include <cmath>
#include "GameState.h"
#include "Evaluation.h"


const bool DEBUG_OUTPUT = true;
unsigned int maxQsearchDepth = 0, currentDepth = 0;
const bool PVSEnabled = 1;

using namespace std;

u8 extractFrom(u32 move)
{
	return (u8)(move & 0b111111);
}
u8 extractTo(u32 move)
{
	return (u8)((move >> 6) & 0b111111);
}

ABAI::ABAI()
{
	cont = new bool;
	*cont = true;
}

ABAI::ABAI(TranspositionTable * tt)
{
	this->tt = tt;
}


ABAI::~ABAI()
{
}

//Does a qSearch
int ABAI::qSearch(int alpha, int beta, bool color, u16 * killerMoves, u32* start, i32 *score)
{
	//currentDepth++;
	//if (currentDepth > maxQsearchDepth)
		//maxQsearchDepth = currentDepth;
	//nodes[0]++;
	int nodeval;
	nodeval = color ? Evaluation::lazyEval(bb) : -Evaluation::lazyEval(bb);
	if (nodeval > alpha)
	{
		alpha = nodeval;
		if (nodeval >= beta)
		{
			//currentDepth--;
			return beta;
		}
	}

	u32 *end;
	//Generate legal QSearch moves for this position
	if (color)
		end = bb->WhiteQSearchMoves(start);
	else
		end = bb->BlackQSearchMoves(start);
	if (*start == 1 || *start == 0)
	{
		//currentDepth--;
		if (nodeval <= alpha)
			return alpha;
		else if (nodeval >= beta)
			return beta;
		return nodeval;
	}

	//SortMoves(start, end, 0, killerMoves, score);
	sortQMoves(start, end, killerMoves, score);
	i32 *nextScore = score + (start - end);
	//SortMoves(start, end, 0, killerMoves);
	u32 move;
	color = !color;
	while (start != end)
	{
		move = *start;
		start++;
		score++;
		bb->MakeMove(move);
		int scoreE = -qSearch(-beta, -alpha, color, killerMoves + 2, end, nextScore);
		bb->UnMakeMove(move);
		if (scoreE >= beta)
		{
			//currentDepth--;
			if (*killerMoves != ((u16)move & ToFromMask))
			{
				*(killerMoves + 1) = *killerMoves;
				*killerMoves = (u16)move & ToFromMask;
			}
			return beta;
		}
		if (scoreE > alpha)
		{
			alpha = scoreE;
			//if (*killerMoves != move & ToFromMask)
			//{
			//	*(killerMoves + 1) = *killerMoves;
			//	*killerMoves = move & ToFromMask;
			//}

		}
		fetchBest(start, end, score);
	}
	//currentDepth--;
	return alpha;
}

//A nega max implementation of Alpha beta search
int ABAI::negamax(int alpha, int beta, int depth, int maxDepth, bool color, u32 *start, u16 *killerMoves, i32 *moveSortValues)
{
	if (!*cont)
		return 0;
	u32 bestMove = 0;
	nodes[depth]++;
	i32 *scorePTR = moveSortValues;
	//Check whether there is a transposition that can be used for this position
	{
		UnpackedHashEntry potEntry(0, 0, 0, 0, 0, 0);
		if (tt->getFromTT(bb->zoobristKey, &potEntry))
		{
			if (potEntry.generation != generation)
			{
				potEntry.generation = generation;
				tt->insertTT(potEntry);
			}
			if (potEntry.depth >= depth)
			{
				if (potEntry.typeOfNode == 1)
				{
					if (potEntry.score > alpha && potEntry.score < beta)
						return potEntry.score;
					else if (potEntry.score <= alpha)
						return alpha;
					else
						return beta;
				}
				else if (potEntry.typeOfNode == 0 && potEntry.score >= beta)
					return beta;
				else if (potEntry.typeOfNode == 2 && potEntry.score <= alpha)
					return alpha;
			}
			bestMove = potEntry.bestMove;
		}
	}
	if (depth <= 0)
	{
		//return color ? lazyEval(): -lazyEval();
		int value = qSearch(alpha, beta, color, killerMoves, start, moveSortValues);
		if (value >= beta)
		{
			tt->insertTT(UnpackedHashEntry(0, depth, beta, bestMove, bb->zoobristKey, generation));
			return beta;
		}
		else if (value > alpha)
		{
			tt->insertTT(UnpackedHashEntry(1, depth, beta, bestMove, bb->zoobristKey, generation));
			return value;
		}
		else
		{
			tt->insertTT(UnpackedHashEntry(2, depth, beta, bestMove, bb->zoobristKey, generation));
			return alpha;
		}
	}

	short bestScore = -8192;

	u32 *end;
	i16 mvcnt;
	//Generate legal moves for this position
	if (color)
		end = bb->WhiteLegalMoves(start);
	else
		end = bb->BlackLegalMoves(start);
	mvcnt = end - start;
	depth--;

	if (*start == 1)
		return -4095 + maxDepth - depth;
	else if (*start == 0)
		return 0;
	
	bool changeAlpha = false;
	bool firstSearch = true;

	//If a move is part of the principal variation, search that first!
	sortMoves(start, end, bestMove, killerMoves, moveSortValues, color);

	//Go through the legal moves
	int moveHistoryCounter;
	bool search;
	while (start != end)
	{
		search = true;
		u32 move = *start;
		start++;
		scorePTR++;
		bb->MakeMove(move);
		//Check for threefold repetition
		int returned = 0;
		moveHistoryCounter = bb->moveHistoryIndex-1;
		while (moveHistoryCounter > 0 && bb->moveHistory[moveHistoryCounter].isReversible())
		{
			//Found repetition. Return draw
			if (bb->moveHistory[moveHistoryCounter] == bb->zoobristKey)
			{
				returned = 0;
				search = false;
				break;
			}
			moveHistoryCounter--;
		}
		if (search)
		{
			if (firstSearch || !PVSEnabled)
			{
				returned = -negamax(-beta, -alpha, depth, maxDepth, !color, end, killerMoves + 2, moveSortValues + mvcnt);
			}
			else
			{
				returned = -negamax(-alpha - 1, -alpha, depth, maxDepth, !color, end, killerMoves + 2, moveSortValues + mvcnt);
				if (returned > alpha)
					returned = -negamax(-beta, -alpha, depth, maxDepth, !color, end, killerMoves + 2, moveSortValues + mvcnt);
			}
		}
		if (returned > bestScore)
		{
			bestScore = returned;
			bestMove = move;
			if (returned > alpha)
			{
				changeAlpha = true;
				alpha = returned;
				firstSearch = false;
			}
		}
		bb->UnMakeMove(move);
		if (returned >= beta)
		{
			if ((*killerMoves) != ((u16)move & ToFromMask) && ((move >> 29) == 7))
			{
				*(killerMoves + 1) = *killerMoves;
				*killerMoves = (u16)move & ToFromMask;
			}
			if ((move >> 29) == 7)
			{
				history[color][bb->mailBox[extractFrom(move)]][extractTo(move)] += depth * depth;
			}
			tt->insertTT(UnpackedHashEntry(0, depth + 1, bestScore, bestMove, bb->zoobristKey, generation));
			return beta;
		}
		fetchBest(start, end, scorePTR);
	}
	//Create an entry for the transposition table
	if (changeAlpha)
		tt->insertTT(UnpackedHashEntry(1, depth + 1, bestScore, bestMove, bb->zoobristKey, generation));
	else
		tt->insertTT(UnpackedHashEntry(2, depth + 1, alpha, bestMove, bb->zoobristKey, generation));
	return alpha;
}

int ABAI::selfPlay(int depth, int moves, GameState *GameState)
{
	double SumFactor = 0;
	bool player = GameState->board->color;
	int score;
	u32 *MoveStart = MoveArray;
	i32 *moveSortValues = sortArray;
	u16 *KillerMoves;// = new u16[200];
	this->bb = GameState->board;
	for (int i2 = 0; i2 < moves; i2++)
	{
		//resetTT();
		generation = (generation + 1) & 0b111;
		maxQsearchDepth = 0;
		KillerMoves = new u16[200]{ 0 };
		for (size_t i = 0; i < 100; i++)
			nodes[i] = 0;
		int alpha = -8192, beta = 8192;
		for (size_t i = 1; i < depth; i++)
		{
			//Do search
			int windowSizeBeta = 25, windowSizeAlpha = 25;
			score = negamax(alpha, beta, i, i, player, MoveStart, KillerMoves, moveSortValues);
			while (score >= beta || score <= alpha)
			{
				cout << "Did research: Old alpha " << alpha << " old beta " << beta;
				if (score >= beta)
				{
					beta += windowSizeBeta;
					windowSizeBeta *= 2;
					cout << " failed high: new beta " << beta;
				}
				if (score <= alpha)
				{
					alpha -= windowSizeAlpha;
					windowSizeAlpha *= 2;
					cout << " failed low: new alpha " << alpha;
				}
				cout << endl;
				score = negamax(alpha, beta, i, i, player, MoveStart, KillerMoves, moveSortValues);
				//alpha = score - 30, beta = score + 30;
				//score = negamax(-8192, 8192, i, i, player, MoveStart, KillerMoves, moveSortValues);
			}
			//alpha = score - 25, beta = score + 25;
			//score = negamax(-8192, 8192, i, i, player, MoveStart, KillerMoves, moveSortValues);
		}
		UnpackedHashEntry potEntry(0, 0, 0, 0, 0, 0);
		if (!tt->getFromTT(bb->zoobristKey, &potEntry))
			std::cout << "ERROR! Non-PV Node: " << endl;
		bb->MakeMove(potEntry.bestMove);
		SumFactor += nodes[0];
		std::cout << "Node Count: " << (nodes[0]) << " BestMove " << IO::convertMoveToAlg(potEntry.bestMove) << " score " << to_string(score) << " cp" << endl;
		player = GameState->board->color;
		cout << "Maxdepth " << maxQsearchDepth << endl;
		delete[] KillerMoves;
	}
	std::cout << "Average Node Count (log 10): " << log10(SumFactor / moves) << endl;
	std::cout << "Average Node Count: " << ((u64)(SumFactor / moves)) << endl;
	return 0;
}

void ABAI::resetNodes()
{
	//Reset the debug node counter
	for (size_t i = 0; i < 100; i++)
		nodes[i] = 0;
}

//Returns an aproximate score based on material

//Used in eval to extract values from the piece square tables for positional awareness 

void ABAI::resetTT()
{
	//tt->resetTT();
}

//Returns a vector of PV from current position. Under development

int ABAI::search(u8 depth, u8 generation, TranspositionTable *tt, BitBoard *bb, bool color)
{
	this->tt = tt;
	//Standard search
	generation = (generation + 1) & 0b111;

	//Create the static array used for storing legal moves
	u32 *MoveStart = MoveArray;
	i32 *moveSortValues = sortArray;
	u16 *KillerMoves = this->killerMoves;

	this->bb = bb;

	int score;

	score = negamax(-8192, 8192, depth, depth, color, MoveStart, KillerMoves, moveSortValues);
	return score;
}


//Sorts the moves based on Killer moves and hash move
void ABAI::sortMoves(u32 * start, u32 * end, u32 bestMove, u16 *killerMoves, i32 *score, bool color)
{
	/*
	Grab best move from hash
	Check if best move is valid
	Run best move

	Remove best move from move array by swapping it first and iterating pointer

	SSE move sort grabbing
	create array of sse values //preallocated

	taking moves set to see
	killer move taking is set to max(50, see + 50)
	killer moves set to 50
	quiet moves set to - 10000 + some score from history heuristic

	BXXXXXXXXXXXXX


	Bestmove
	SSE +
	Killer moves
	SSE -
	Quiet moves - History Heurestic
	*/
	u32 *OGstart = start, mem, *KMStart = start, *BestMove = start;
	i32 BestScore = -32000, *OGScore = score, *bestScorePTR = score;
	bestMove &= (ToFromMask);
	u16 KM1 = *killerMoves, KM2 = *(killerMoves + 1);
	while (start < end)
	{
		u16 move = (*start) & ToFromMask;
		if (move == bestMove)
			*score = (i32)((((u64)1) << 31) - 1);
		else if (((*start) >> 29) == 7)//bb->mailBox[move >> 6] == 14)
									   //	//((*start) >> 29) != 7)
		{
			if (move == KM1)
				*score = -1048576 + 10000 +history[color][bb->mailBox[extractFrom(move)]][extractTo(move)];
			else if (move == KM2)
				*score = -1048576 + 9999 +history[color][bb->mailBox[extractFrom(move)]][extractTo(move)];
			else
				*score = -1048576 + 10 +history[color][bb->mailBox[extractFrom(move)]][extractTo(move)];
		}
		else
		{
			//*score = (getPieceValue(bb->mailBox[extractTo(move)]) - (getPieceValue(bb->mailBox[extractFrom(move)]) >> 3)) * 1048576;
			*score = ((u32)(bb->SEEWrapper(*start))) * 1048576;
			//*score = 50;
		}
		if (*score > BestScore)
		{
			BestScore = *score;
			bestScorePTR = score;
			BestMove = start;
		}
		start++;
		score++;
	}
	u32 temp = *OGstart;
	*OGstart = *BestMove;
	*BestMove = temp;
	*bestScorePTR = *OGScore;
}

void ABAI::sortQMoves(u32 * start, u32 * end, u16 * killerMoves, i32 * score)
{
	u32 *OGStart = start, *bestMove = start;
	i32 *OGScore = score, *BestScore = score;
	u16 km1 = (*killerMoves) & ToFromMask, km2 = (*(killerMoves + 1)) & ToFromMask, move;
	while (start < end)
	{
		move = (*start) & ToFromMask;
		if (move == km1 || move == km2)
			*score = 32000;
		else
			*score = Evaluation::getPieceValue(bb->mailBox[extractTo(move)]) - (Evaluation::getPieceValue(bb->mailBox[extractFrom(move)]) >> 2);
		if (*score > *BestScore)
		{
			bestMove = start;
			BestScore = score;
		}
		start++;
		score++;
	}
	u32 temp1 = *bestMove;
	*BestScore = *OGScore;
	*bestMove = *OGStart;
	*OGStart = temp1;
}

void ABAI::fetchBest(u32 * start, u32 * end, i32 * score)
{
	//score++;
	u32 *ogStart = start, *bestMove = start;
	i32 ogScore = *score, *bestScore = score;
	while (start != end)
	{
		if (*bestScore < *score)
		{
			bestScore = score;
			bestMove = start;
		}
		start++;
		score++;
	}
	u32 temp = *bestMove;
	*(bestMove) = *ogStart;
	*ogStart = temp;
	*bestScore = ogScore;
}
