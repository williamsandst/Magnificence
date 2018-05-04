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
	tt.setHashSizeBits(22);
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
	u32 bestMove = 0;
	nodes[depth]++;
	i32 *scorePTR = moveSortValues;
	//Check whether there is a transposition that can be used for this position
	{
		UnpackedHashEntry potEntry(0, 0, 0, 0, 0, 0);
		if (tt.getFromTT(bb->zoobristKey, &potEntry))
		{
			if (potEntry.generation != generation)
			{
				potEntry.generation = generation;
				tt.insertTT(potEntry);
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
			tt.insertTT(UnpackedHashEntry(0, depth, beta, bestMove, bb->zoobristKey, generation));
			return beta;
		}
		else if (value > alpha)
		{
			tt.insertTT(UnpackedHashEntry(1, depth, beta, bestMove, bb->zoobristKey, generation));
			return value;
		}
		else
		{
			tt.insertTT(UnpackedHashEntry(2, depth, beta, bestMove, bb->zoobristKey, generation));
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
			tt.insertTT(UnpackedHashEntry(0, depth + 1, bestScore, bestMove, bb->zoobristKey, generation));
			return beta;
		}
		fetchBest(start, end, scorePTR);
	}
	//Create an entry for the transposition table
	if (changeAlpha)
		tt.insertTT(UnpackedHashEntry(1, depth + 1, bestScore, bestMove, bb->zoobristKey, generation));
	else
		tt.insertTT(UnpackedHashEntry(2, depth + 1, alpha, bestMove, bb->zoobristKey, generation));
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
		if (!tt.getFromTT(bb->zoobristKey, &potEntry))
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

//Returns an aproximate score based on material

//Used in eval to extract values from the piece square tables for positional awareness 

void ABAI::resetTT()
{
	tt.resetTT();
}

//Returns a vector of PV from current position. Under development

vector<u32> ABAI::search(GameState &gameState)
{
	//Standard search
	generation = (generation + 1) & 0b111;

	//Create the static array used for storing legal moves
	u32 *MoveStart = MoveArray;
	i32 *moveSortValues = sortArray;
	u16 *KillerMoves = new u16[200];

	this->bb = gameState.board;
	vector<u32> PV;

	//Reset the debug node counter
	for (size_t i = 0; i < 100; i++)
		nodes[i] = 0;

	//Variables used for debugging
	clock_t start = clock();

	int score;

	u32 pV[100];

	cout << endl;

	score = negamax(-8192, 8192, gameState.maxDepth, gameState.maxDepth, gameState.color, MoveStart, KillerMoves, moveSortValues);

	clock_t end = clock();

	cout << "info depth " << to_string(gameState.maxDepth) << " score cp " << to_string(score) << " pv ";
	//Extract PV
	for (size_t i = 0; i < gameState.maxDepth; i++)
	{
		UnpackedHashEntry potEntry(0, 0, 0, 0, 0, 0);
		if (!tt.getFromTT(bb->zoobristKey, &potEntry))
			cout << "ERROR! Non-PV Node: " << endl;
		pV[i] = potEntry.bestMove;
		PV.push_back(pV[i]);
		cout << IO::convertMoveToAlg(pV[i]) << " ";
		bb->MakeMove(pV[i]);
	}
	//Unmake pV
	for (size_t i = 1; i < gameState.maxDepth + 1; i++)
	{
		bb->UnMakeMove(pV[gameState.maxDepth - i]);
	}
	cout << endl;


	if (DEBUG_OUTPUT)
	{
		cout << endl << "Score: " << to_string(score) << " at depth " << to_string(gameState.maxDepth) << endl;
		cout << to_string(nodes[0]) << " nodes in " << to_string((((end - start) / double CLOCKS_PER_SEC))) << " s [" <<
			to_string(nodes[0] / (((end - start) / double CLOCKS_PER_SEC) * 1000000)) << " Mpos/sec]" << endl;
		cout << "Branching factor: " << pow(nodes[0], (float)1 / (float)gameState.maxDepth) << endl;

		cout << "Branching factors: ";
		for (size_t i = 0; i < gameState.maxDepth - 1; i++)
		{
			cout << endl << to_string(nodes[i]) << " / " << to_string(nodes[i + 1]) << " = ";
			cout << to_string(gameState.maxDepth - i) << "/" << to_string(gameState.maxDepth - i - 1) << ": " << to_string((float)nodes[i] / (float)nodes[i + 1])
				<< ", ";
		}
		cout << endl;
	}

	//Memory cleanup
	delete[] KillerMoves;

	return PV;
}

vector<u32> ABAI::searchID(GameState &gameState)
{
	//Standard search
	generation = (generation + 1) & 0b111;

	//Create the static array used for storing legal moves
	u32 *MoveStart = MoveArray;
	i32 *moveSortValues = sortArray;
	u16 *KillerMoves = new u16[200];

	this->bb = gameState.board;
	vector<u32> PV;

	//Reset the debug node counter
	for (size_t i = 0; i < 100; i++)
		nodes[i] = 0;

	//Variables used for debugging
	clock_t start = clock();

	int score;

	u32 pV[100];

	cout << endl;
	int alpha = -8192;
	int beta = 8192;
	for (int i = 1; i < gameState.maxDepth + 1; i++)
	{
		//Do search
		score = negamax(alpha, beta, i, i, gameState.color, MoveStart, KillerMoves, moveSortValues);
		if (score <= alpha || score >= beta) //If the search is out of bounds
		{
			//Another search is needed with a full window
			if (DEBUG_OUTPUT)
			{
				cout << "Window re-search needed" << endl;
			}
			score = negamax(-8192, 8192, i, i, gameState.color, MoveStart, KillerMoves, moveSortValues);
		}
		//alpha = score - 10;
		//beta = score + 10;
		cout << "info depth " << to_string(i) << " score cp " << to_string(score) << " pv ";
		for (size_t i2 = 0; i2 < i; i2++)
		{
			UnpackedHashEntry potEntry(0, 0, 0, 0, 0, 0);
			if (!tt.getFromTT(bb->zoobristKey, &potEntry))
				cout << "ERROR! Non-PV Node: " << endl;
			pV[i2] = potEntry.bestMove;
			PV.push_back(pV[i2]);
			cout << IO::convertMoveToAlg(pV[i2]) << " ";
			bb->MakeMove(pV[i2]);
		}
		//Unmake pV
		for (size_t i2 = 1; i2 < i + 1; i2++)
		{
			bb->UnMakeMove(pV[i - i2]);
		}
		cout << endl;
	}
	clock_t end = clock();

	if (DEBUG_OUTPUT)
	{
		cout << endl << "Score: " << to_string(score) << " at depth " << to_string(gameState.maxDepth) << endl;
		cout << to_string(nodes[0]) << " nodes in " << to_string((((end - start) / double CLOCKS_PER_SEC))) << " s [" <<
			to_string(nodes[0] / (((end - start) / double CLOCKS_PER_SEC) * 1000000)) << " Mpos/sec]" << endl;
		cout << "Branching factor: " << pow(nodes[0], (float)1 / (float)gameState.maxDepth) << endl;

		cout << "Branching factors: ";
		for (size_t i = 0; i < gameState.maxDepth - 1; i++)
		{
			cout << endl << to_string(nodes[i]) << " / " << to_string(nodes[i + 1]) << " = ";
			cout << to_string(gameState.maxDepth - i) << "/" << to_string(gameState.maxDepth - i - 1) << ": " << to_string((float)nodes[i] / (float)nodes[i + 1])
				<< ", ";
		}
	}

	cout << endl;

	//Memory cleanup
	delete[] KillerMoves;

	return PV;
}

vector<u32> ABAI::searchIDSimpleTime(GameState &gameState)
{
	//Standard search
	//resetTT();
	generation = (generation + 1) & 0b111;

	//Create the static array used for storing legal moves
	u32 *MoveStart = MoveArray;
	i32 *moveSortValues = sortArray;
	u16 *KillerMoves = new u16[200];

	this->bb = gameState.board;
	vector<u32> PV;

	//Reset the debug node counter
	for (size_t i = 0; i < 100; i++)
		nodes[i] = 0;

	//Variables used for debugging
	clock_t start = clock();

	int score;

	u32 pV[100];

	cout << endl;

	double totalTime;
	double branchingFactor;
	const int maxTime = 20;
	bool runSearch = true;
	cout << endl;
	int i = 1;
	while (runSearch)
	{
		//Do search
		PV.clear();
		score = negamax(-8192, 8192, i, i, gameState.color, MoveStart, KillerMoves, moveSortValues);
		clock_t timerEnd = clock();
		totalTime = (timerEnd - start) / double CLOCKS_PER_SEC;
		branchingFactor = pow(nodes[0], 1 / (double)i);
		if (totalTime * branchingFactor > gameState.maxTime)
			runSearch = false;
		//Information generation
		cout << "info depth " << to_string(i) << " score cp " << to_string(score) << " pv ";
		//Find pV
		for (size_t i2 = 0; i2 < i; i2++)
		{
			UnpackedHashEntry potEntry(0, 0, 0, 0, 0, 0);
			if (!tt.getFromTT(bb->zoobristKey, &potEntry))
				cout << "ERROR! Non-PV Node: " << endl;
			pV[i2] = potEntry.bestMove;
			PV.push_back(pV[i2]);
			cout << IO::convertMoveToAlg(pV[i2]) << " ";
			bb->MakeMove(pV[i2]);
		}
		//Unmake pV
		for (size_t i2 = 1; i2 < i + 1; i2++)
		{
			bb->UnMakeMove(pV[i - i2]);
		}
		cout << endl;
		i++;
	}

	int maxDepth = i-1;

	clock_t end = clock();

	if (DEBUG_OUTPUT)
	{
		cout << endl << "Score: " << to_string(score) << " at depth " << to_string(maxDepth) << endl;
		cout << to_string(nodes[0]) << " nodes in " << to_string((((end - start) / double CLOCKS_PER_SEC))) << " s [" <<
			to_string(nodes[0] / (((end - start) / double CLOCKS_PER_SEC) * 1000000)) << " Mpos/sec]" << endl;
		cout << "Branching factor: " << pow(nodes[0], (float)1 / (float)maxDepth) << endl;

		cout << "Branching factors: ";
		for (size_t i = 0; i < maxDepth - 1; i++)
		{
			cout << endl << to_string(nodes[i]) << " / " << to_string(nodes[i + 1]) << " = ";
			cout << to_string(maxDepth - i) << "/" << to_string(maxDepth - i - 1) << ": " << to_string((float)nodes[i] / (float)nodes[i + 1])
				<< ", ";
		}
	}

	cout << endl;

	//Memory cleanup
	delete[] KillerMoves;

	return PV;
}

vector<u32> ABAI::searchIDComplexTime(GameState &gameState)
{
	//Standard search
	generation = (generation + 1) & 0b111;

	//Create the static array used for storing legal moves
	u32 *MoveStart = MoveArray;
	i32 *moveSortValues = sortArray;
	u16 *KillerMoves = new u16[200];

	this->bb = gameState.board;
	vector<u32> PV;

	//Reset the debug node counter
	for (size_t i = 0; i < 100; i++)
		nodes[i] = 0;

	//Variables used for debugging
	clock_t start = clock();

	int score;

	u32 pV[100];

	cout << endl;

	UnpackedHashEntry q(0, 0, 0, 0, 0, 0);
	int depth = 0;
	u32 bestMove = 0;
	if (tt.getFromTT(bb->zoobristKey, &q))
	{
		depth = q.depth;
		gameState.maxDepth = depth;
		score = q.score;
		q.generation = generation;
		tt.insertTT(q);
	}
	u32 *startM = MoveStart, *endM;
	if (gameState.color)
		endM = bb->WhiteLegalMoves(startM);
	else
		endM = bb->WhiteLegalMoves(startM);
	u16 *mockKillerMoves = new u16[2]{ 0, 0 };
	u8 or = 0;
	int nrNodes = 0;
	while (clock() - start < gameState.maxDepth)
	{
		PV.clear();
		int alpha = -8192, beta = 8192;
		startM = MoveStart;
		u32 move;
		//SortMoves(startM, endM, bestMove, mockKillerMoves);
		while (startM != endM && clock() - start < gameState.maxDepth)
		{
			move = *startM;
			startM++;
			bb->MakeMove(move);
			int returned = -negamax(-beta, -alpha, depth, depth, !gameState.color, endM, KillerMoves, moveSortValues);
			bb->UnMakeMove(move);
			if (returned > alpha)
			{
				if (mockKillerMoves[0] != ((u16)move & ToFromMask))
				{
					mockKillerMoves[1] = mockKillerMoves[0];
					mockKillerMoves[0] = (u16)move & ToFromMask;
				}
				alpha = returned;
				bestMove = move;
			}
		}
		if (clock() - start < gameState.maxTime)
		{
			tt.insertTT(UnpackedHashEntry(1, depth + 1, alpha, bestMove, bb->zoobristKey, generation));
			score = alpha;
			gameState.maxDepth = depth + 1;
		}
		cout << "info depth " << to_string(depth) << " score cp " << to_string(score) << " pv ";
		for (size_t i2 = 0; i2 < depth; i2++)
		{
			UnpackedHashEntry potEntry(0, 0, 0, 0, 0, 0);
			if (!tt.getFromTT(bb->zoobristKey, &potEntry))
				cout << "ERROR! Non-PV Node: " << endl;
			pV[i2] = potEntry.bestMove;
			PV.push_back(pV[i2]);
			cout << IO::convertMoveToAlg(pV[i2]) << " ";
			bb->MakeMove(pV[i2]);
		}
		//Unmake pV
		for (size_t i2 = 1; i2 < depth + 1; i2++)
		{
			bb->UnMakeMove(pV[depth - i2]);
		}
		cout << endl;
		depth++;
	}
	delete[] mockKillerMoves;

	int maxDepth = depth - 1;

	clock_t end = clock();

	if (DEBUG_OUTPUT)
	{
		cout << endl << "Score: " << to_string(score) << " at depth " << to_string(maxDepth) << endl;
		cout << to_string(nodes[0]) << " nodes in " << to_string((((end - start) / double CLOCKS_PER_SEC))) << " s [" <<
			to_string(nodes[0] / (((end - start) / double CLOCKS_PER_SEC) * 1000000)) << " Mpos/sec]" << endl;
		cout << "Branching factor: " << pow(nodes[0], (float)1 / (float)maxDepth) << endl;

		cout << "Branching factors: ";
		for (size_t i = 0; i < maxDepth - 1; i++)
		{
			cout << endl << to_string(nodes[i]) << " / " << to_string(nodes[i + 1]) << " = ";
			cout << to_string(maxDepth - i) << "/" << to_string(maxDepth - i - 1) << ": " << to_string((float)nodes[i] / (float)nodes[i + 1])
				<< ", ";
		}
	}

	cout << endl;

	//Memory cleanup
	delete[] KillerMoves;

	return PV;
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
