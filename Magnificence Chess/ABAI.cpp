#include "stdafx.h"
#include "ABAI.h"
#pragma once
#include "IO.h"
#include <cmath>
#include "GameState.h"


const bool DEBUG_OUTPUT = true;

using namespace std;

ABAI::ABAI()
{

}


ABAI::~ABAI()
{
}

//adds a board position to the transposition table
int ABAI::insertTT(PackedHashEntry newEntry)
{
	u32 index = (u32)(extractKey(newEntry) & hashMask);
	//if both type 0 and generation different or depth lower or 
	if ((extractNodeType(ttDepthFirst[index]) != 1 || extractNodeType(newEntry) == 1 || extractGeneration(newEntry) != extractGeneration(ttDepthFirst[index])) &&
		(extractDepth(ttDepthFirst[index]) <= extractDepth(newEntry) || extractGeneration(ttDepthFirst[index]) != extractGeneration(newEntry)))
	{
		ttDepthFirst[index] = newEntry;
	}
	else
		ttAlwaysOverwrite[index] = newEntry;
	return extractDepth(newEntry);
}

//Checks if a board position is in the transposition table
bool ABAI::getFromTT(u64 key, UnpackedHashEntry *in)
{
	u32 index = (u32)(key & hashMask);
	PackedHashEntry entry = ttDepthFirst[index];
	if (extractKey(entry) == key)
	{
		*in = UnpackedHashEntry(entry);
		return true;
	}
	entry = ttAlwaysOverwrite[index];
	if (extractKey(entry) == key)
	{
		*in = UnpackedHashEntry(entry);
		return true;
	}
	else
		return false;
}

//Does a qSearch
int ABAI::qSearch(int alpha, int beta, bool color, u16 * killerMoves, u32* start, i16 *score)
{
	int nodeval;
	if (color)
		nodeval = lazyEval();
	else
		nodeval = -lazyEval();
	if (nodeval > alpha)
	{
		alpha = nodeval;
		if (nodeval > beta)
		{
			return beta;
		}
	}
	u32 *end;
	//Generate legal QSearch moves for this position
	if (color)
		end = bb->WhiteQSearchMoves(start);
	else
		end = bb->BlackQSearchMoves(start);
	//SortMoves(start, end, 0, killerMoves, score);
	i16 *nextScore = score + (start - end);
	if (*start == 1 || *start == 0)
		return nodeval;
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
			if (*killerMoves != ((u16)move & ToFromMask))
			{
				*(killerMoves + 1) = *killerMoves;
				*killerMoves = (u16)move & ToFromMask;
			}
			return beta;
		}
		if (scoreE > alpha)
			alpha = scoreE;
		//FetchBest(start, end, score);
	}
	return alpha;
}

//A nega max implementation of Alpha beta search
int ABAI::negamax(int alpha, int beta, int depth, int maxDepth, bool color, u32 *start, u16 *killerMoves, i16 *moveSortValues)
{
	u32 bestMove = 0;
	nodes[depth]++;
	i16 *scorePTR = moveSortValues;
	//Check whether there is a transposition that can be used for this position
	{
		UnpackedHashEntry potEntry(0, 0, 0, 0, 0, 0);
		if (getFromTT(bb->zoobristKey, &potEntry))
		{
			if (potEntry.generation != generation)
			{
				potEntry.generation = generation;
				insertTT(potEntry);
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
			insertTT(UnpackedHashEntry(0, depth, beta, bestMove, bb->zoobristKey, generation));
			return beta;
		}
		else if (value > alpha)
		{
			insertTT(UnpackedHashEntry(1, depth, beta, bestMove, bb->zoobristKey, generation));
			return value;
		}
		else
		{
			insertTT(UnpackedHashEntry(2, depth, beta, bestMove, bb->zoobristKey, generation));
			return alpha;
		}
		return value;
	}

	short bestScore = -8192;

	u32 *end;
	i16 mvcnt;
	//Generate legal moves for this position
	if (color)
		end = bb->WhiteLegalMoves(start);
	else
		end = bb->BlackLegalMoves(start);
	if (*start == 1)
		return -4095 + maxDepth - depth;
	else if (*start == 0)
		return 0;
	mvcnt = end - start;
	depth--;
	color = !color;

	//If a move is part of the principal variation, search that first!
	sortMoves(start, end, bestMove, killerMoves, moveSortValues);
	
	if (*start == 0)
		return 0;
	else if (*start == 1)
		return -4095 + maxDepth - depth;
	//Go through the legal moves
	while (start != end)
	{
		u32 move = *start;
		start++;
		scorePTR++;
		if (move != 0 && move != 1)
		{
			bb->MakeMove(move);
			int returned = -negamax(-beta, -alpha, depth, maxDepth, color, end, killerMoves + 2, moveSortValues + mvcnt + 1);
			if (returned > bestScore)
			{
				bestScore = returned;
				bestMove = move;
			}
			bb->UnMakeMove(move);
			if (returned >= beta)
			{
				if ((*killerMoves) != ((u16)move & ToFromMask) && ((move >> 29) == 7))
				{
					*(killerMoves + 1) = *killerMoves;
					*killerMoves = (u16)move & ToFromMask;
				}
				insertTT(UnpackedHashEntry(0, depth + 1, bestScore, bestMove, bb->zoobristKey, generation));
				return beta;
			}
			if (returned > alpha)
			{
				alpha = returned;
			}
		}
		else if (move == 1) //If in check, return a very bad score
			return -4095 + maxDepth - depth;
		else
			return 0; //If draw, return 0
		fetchBest(start, end, scorePTR);
	}
	//Create an entry for the transposition table
	if (alpha == bestScore)
		insertTT(UnpackedHashEntry(1, depth + 1, bestScore, bestMove, bb->zoobristKey, generation));
	else
		insertTT(UnpackedHashEntry(2, depth + 1, bestScore, bestMove, bb->zoobristKey, generation));
	return alpha;
}

//Returns an aproximate score based on material
int ABAI::lazyEval()
{
	//nodes[0]++;
	short score = 0;
	score += bb->pc(bb->Pieces[5]) * Pawn + bb->pc(bb->Pieces[4]) * Knight + bb->pc(bb->Pieces[3]) * Rook + bb->pc(bb->Pieces[2]) * Bishop + bb->pc(bb->Pieces[1]) * Queen
		- bb->pc(bb->Pieces[12]) * Pawn - bb->pc(bb->Pieces[11]) * Knight - bb->pc(bb->Pieces[10]) * Rook - bb->pc(bb->Pieces[9]) * Bishop - bb->pc(bb->Pieces[8]) * Queen;
	score += pieceSquareValues(whitePawnEarlyPST, bb->Pieces[5]);
	score -= pieceSquareValues(blackPawnEarlyPST, bb->Pieces[12]);
	score += pieceSquareValues(whiteKnightEarlyPST, bb->Pieces[4]);
	score -= pieceSquareValues(blackKnightEarlyPST, bb->Pieces[11]);
	if (bb->color) score += 40;
	return score;
}

//Used in eval to extract values from the piece square tables for positional awareness 
int ABAI::pieceSquareValues(const short * pieceSquareTable, u64 pieceSet)
{
	u32 index;
	short sum = 0;
	while (pieceSet)
	{
		_BitScanForward64(&index, pieceSet);
		pieceSet &= pieceSet - 1;
		sum += pieceSquareTable[index];
	}
	return sum;
}

void ABAI::resetTT()
{
	int size = 24 + 1; //bits
	int i = 1;
	while ((size -= 1) && (i *= 2));
	//cout << i << endl;
	ttAlwaysOverwrite = new PackedHashEntry[i];
	ttDepthFirst = new PackedHashEntry[i];
	hashMask = i - 1;
}

//Returns a vector of PV from current position. Under development

vector<u32> ABAI::bestMove(GameState &gameState)
{
	/*
	To do
	//0.	Q-search
	//1.	Iterative Depening
	2.	History Heurestic
	3.	Move Sorting
	4.	Multithreading
	6.	PV search
	7.	Lazy eval into make unmake move
	8.	More heuristics
	*/
	generation = (generation + 1) & 0b111;

	//Create the static array used for storing legal moves
	u32 *MoveStart = MoveArray;
	i16 *moveSortValues = sortArray;
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

	//Run search
	//Iterative deepening

	cout << endl;
	for (int i = 1; i < gameState.maxDepth + 1; i++)
	{
		//Do search
		score = negamax(-8192, 8192, i, i, gameState.color, MoveStart, KillerMoves, moveSortValues);
		cout << "info depth " << to_string(i) << " score cp " << to_string(score) << " pv ";
		for (size_t i2 = 0; i2 < i; i2++)
		{
			UnpackedHashEntry potEntry(0, 0, 0, 0, 0, 0);
			if (!getFromTT(bb->zoobristKey, &potEntry))
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


	//Normal search
	//score = negamax(-8192, 8192, maxDepth, maxDepth, color, MoveStart, KillerMoves);

	//Iterative deepening with time limit. William version
	/*double totalTime;
	double branchingFactor;
	const int maxTime = 20;
	bool runSearch = true;
	cout << endl;
	int i = 1;
	while (runSearch)
	{
		//Do search
		PV.clear();
		score = negamax(-8192, 8192, i, i, color, MoveStart, KillerMoves);
		clock_t timerEnd = clock();
		totalTime = (timerEnd - start) / double CLOCKS_PER_SEC;
		branchingFactor = pow(nodes[0], 1 / (double)i);
		if (totalTime * branchingFactor > maxTime)
			runSearch = false;
		//Information generation
		cout << "info depth " << to_string(i) << " score cp " << to_string(score) << " pv ";
		//Find pV
		for (size_t i2 = 0; i2 < i; i2++)
		{
			UnpackedHashEntry potEntry(0, 0, 0, 0, 0, 0);
			if (!getFromTT(bb->zoobristKey, &potEntry))
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
	*/
	//Iterative deepening with time limit. Haralds version; broken?
	/*UnpackedHashEntry q(0, 0, 0, 0, 0, 0);
	int depth = 0;
	u32 bestMove = 0;
	if (getFromTT(bb->zoobristKey, &q))
	{
		depth = q.depth;
		maxDepth = depth;
		score = q.score;
		q.generation = generation;
		insertTT(q);
	}
	u32 *startM = MoveStart, *endM;
	if (color)
		endM = bb->WhiteLegalMoves(startM);
	else
		endM = bb->WhiteLegalMoves(startM);
	u16 *mockKillerMoves = new u16[2]{ 0, 0 };
	u8 or = 0;
	int nrNodes = 0;
	while (clock() - start < time)
	{
		PV.clear();
		int alpha = -8192, beta = 8192;
		startM = MoveStart;
		u32 move;
		SortMoves(startM, endM, bestMove, mockKillerMoves);
		while (startM != endM && clock() - start < time)
		{
			move = *startM;
			startM++;
			bb->MakeMove(move);
			int returned = -negamax(-beta, -alpha, depth, depth, !color, endM, KillerMoves);
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
		if (clock() - start < time)
		{
			insertTT(UnpackedHashEntry(1, depth + 1, alpha, bestMove, bb->zoobristKey, generation));
			score = alpha;
			maxDepth = depth + 1;
		}
		cout << "info depth " << to_string(depth) << " score cp " << to_string(score) << " pv ";
		for (size_t i2 = 0; i2 < depth; i2++)
		{
			UnpackedHashEntry potEntry(0, 0, 0, 0, 0, 0);
			if (!getFromTT(bb->zoobristKey, &potEntry))
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
	delete[] mockKillerMoves;*/
	clock_t end = clock();

	//Debug output
	
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
	//cout << endl << "Principal variation (tri-pV table): ";
	/*for (size_t i = 0; i < maxDepth; i++)
	{
		//PV.push_back(triangularPV[i]);
		cout << IO::convertMoveToAlg(triangularPV[i]) << " ";
	}*/
	//u32 *pV = new u32[100];
	//Find pV variation from transposition table
	//cout << endl << "Principal variation (TT): ";
	cout << endl;
	//Undo the pV moves

	//Memory cleanup
	delete[] KillerMoves;

	return PV;
}

vector<u32> ABAI::search(GameState &gameState)
{
	//Standard search
	generation = (generation + 1) & 0b111;

	//Create the static array used for storing legal moves
	u32 *MoveStart = MoveArray;
	i16 *moveSortValues = sortArray;
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
		if (!getFromTT(bb->zoobristKey, &potEntry))
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
	i16 *moveSortValues = sortArray;
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

	for (int i = 1; i < gameState.maxDepth + 1; i++)
	{
		//Do search
		score = negamax(-8192, 8192, i, i, gameState.color, MoveStart, KillerMoves, moveSortValues);
		cout << "info depth " << to_string(i) << " score cp " << to_string(score) << " pv ";
		for (size_t i2 = 0; i2 < i; i2++)
		{
			UnpackedHashEntry potEntry(0, 0, 0, 0, 0, 0);
			if (!getFromTT(bb->zoobristKey, &potEntry))
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
	generation = (generation + 1) & 0b111;

	//Create the static array used for storing legal moves
	u32 *MoveStart = MoveArray;
	i16 *moveSortValues = sortArray;
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
			if (!getFromTT(bb->zoobristKey, &potEntry))
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
	i16 *moveSortValues = sortArray;
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
	if (getFromTT(bb->zoobristKey, &q))
	{
		depth = q.depth;
		gameState.maxDepth = depth;
		score = q.score;
		q.generation = generation;
		insertTT(q);
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
			insertTT(UnpackedHashEntry(1, depth + 1, alpha, bestMove, bb->zoobristKey, generation));
			score = alpha;
			gameState.maxDepth = depth + 1;
		}
		cout << "info depth " << to_string(depth) << " score cp " << to_string(score) << " pv ";
		for (size_t i2 = 0; i2 < depth; i2++)
		{
			UnpackedHashEntry potEntry(0, 0, 0, 0, 0, 0);
			if (!getFromTT(bb->zoobristKey, &potEntry))
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

//Generates a packed hash entry from an unpacked hash entry
PackedHashEntry::PackedHashEntry(UnpackedHashEntry start)
{
	//data needs to store the best move, the score of the node, 
	//the type of node (lowest value possible value, highest possible value, exact value)
	//depth searched
	//score is in centipawns, the highest possible score is 3900 only counting piece score, it is signed. That represents 13 bits (+- 2^12 - 1 = +-4095)
	//therefore 16 bits will be assigned to score because that is a short (+-(2^ 15 - 1) = +- 32767) to allow for something unexpected
	//64 - 16 = 48;
	//type of node takes exactly 3 bits. 48 - 3 = 45;
	//The move is 32 bit and can be stored as such. 47 - 32 = 12;
	//generation is the point when the node was created. It is updated by generation = (generation + 1) & 0b111
	//generation is 2 bits. 12 - 3 = 9;
	//10 ^ 2 - 1 = 1023 which is more than sufficient for the depht.
	data = ((u64)((u16)start.score)) | (((u64)start.bestMove) << 16) | (((u64)start.typeOfNode) << 48) | (((u64)start.generation) << 51) | (((u64)start.depth) << 54);
	key = start.key ^ data;
}

//generates an empty packed hash entry
PackedHashEntry::PackedHashEntry()
{
	key = 0;
	data = 0;
}

//generates an unpacked hash entry
//type of node: The type of node 0 is a minimum value, 1 is an exact value, 2 is a maximum value
//Depth: the depth it was searched at
//Score: The score returned
//Bestmove: The move causing the highest score
//Key: The zoobrist hash of position
//Generation: A variable showing roughly when it was searched
UnpackedHashEntry::UnpackedHashEntry(u8 typeOfNode, u16 depth, short score, u32 bestMove, u64 key, u8 generation)
{
	this->typeOfNode = typeOfNode; //(0 is minimum value, 1 is exact, 2 is maximum value)
	this->depth = depth;
	this->score = score;
	this->bestMove = bestMove;
	this->key = key;
	this->generation = generation;
}


//Decodes a packed hash entry into a unpacked hash entry
UnpackedHashEntry::UnpackedHashEntry(PackedHashEntry in)
{
	score = (short)(0xffff & in.data);
	bestMove = (u32)(0xffffffff & (in.data >> 16));
	typeOfNode = (u8)(((u8)0b11) & (in.data >> 48));
	generation = (u8)(((u8)0b111) & in.data >> 51);
	depth = (short)(in.data >> 54);
	key = in.key ^ in.data;
}

//extracts the type of node from a packed hash entrt
//0 is minimum value, 1 is exact value and 2 is maximum value
u8 ABAI::extractNodeType(PackedHashEntry in)
{
	return (u8)(((u8)0b11) & (in.data >> 48));
}

//Extracts the depth from a packed hash entry
short ABAI::extractDepth(PackedHashEntry in)
{
	return (short)(in.data >> 54);
}

//Extracts the score from a packed hash entry
short ABAI::extractScore(PackedHashEntry in)
{
	return (short)(0xffff & in.data);
}

//Sorts the moves based on Killer moves and hash move
void ABAI::sortMoves(u32 * start, u32 * end, u32 bestMove, u16 *killerMoves, i16 *score)
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
	i16 BestScore = -32000, *OGScore = score, *bestScorePTR = score;
	bestMove &= (ToFromMask);
	u16 KM1 = *killerMoves & ToFromMask, KM2 = *(killerMoves + 1) & ToFromMask;
	while (start < end)
	{
		u16 move = (*start) & ToFromMask;
		if (move == bestMove)
			*score = 32000;
		else if (move == KM1 || move == KM2)
			*score = 2;
		else if (((*start) >> 29) == 7)//bb->mailBox[move >> 6] == 14)
			//	//((*start) >> 29) != 7)
			*score = -1;
		else
		{
			*score = bb->SEEWrapper(*start);
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

void ABAI::fetchBest(u32 * start, u32 * end, i16 * score)
{
	//score++;
	u32 *ogStart = start, *bestMove = start;
	i16 ogScore = *score, *bestScore = score;
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

//extracts the bestmove from a packed hash entry
u32 ABAI::extractBestMove(PackedHashEntry in)
{
	return (u32)(0xffffffff & (in.data >> 16));
}

//extracts the zoobrist hash from a packed hash entry
u64 ABAI::extractKey(PackedHashEntry in)
{
	return in.key ^ in.data;
}

//extracts generation from a packed hash entry
u8 ABAI::extractGeneration(PackedHashEntry in)
{
	return (u8)(((u8)0b111) & in.data >> 51);
}
