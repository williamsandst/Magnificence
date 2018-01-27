#include "stdafx.h"
#include "Engine.h"
#include "ArrayBoard.h";
#include "Move.h";
#include <iostream>
#include "IO.h"
#include <ctime>

int nodes[7];

int Engine::evaluateBoard(ArrayBoard & board)
{
	int evaluation = 0;
	for (size_t i = 0; i < 64; i++)
	{
		switch (board.board[i])
		{
		case 0:
			break;
		case 1:
			evaluation += pawnValue;
			break;
		case 2:
			evaluation += rookValue;
			break;
		case 3:
			evaluation += knightValue;
			break;
		case 4:
			evaluation += bishopValue;
			break;
		case 5:
			evaluation += queenValue;
			break;
		case 11:
			evaluation -= pawnValue;
			break;
		case 12:
			evaluation -= rookValue;
			break;
		case 13:
			evaluation -= knightValue;
			break;
		case 14:
			evaluation -= bishopValue;
			break;
		case 15:
			evaluation -= queenValue;
			break;
		}
	}
	return evaluation;
}

void Engine::movcpy(__int16* pTarget, const __int16* pSource, int n) {
	while (n-- && (*pTarget++ = *pSource++));
}

int Engine::negaMax(ArrayBoard board, int depth, int ply, int maxDepth, __int16 triangularPVTable[], short pvIndex)
{
	//Normal minmax
	nodes[depth]++;
	if (depth == 0)
		return board.whiteTurn ? evaluateBoard(board) : -evaluateBoard(board);
	triangularPVTable[pvIndex] = 0; //No principal variation found yet
	int pvNextIndex = pvIndex + maxDepth - ply;
	vector<__int16> moves;
	if (board.whiteTurn)
		moves = board.generateWhiteLegalMoves();
	else
		moves = board.generateBlackLegalMoves();
	board.totalPly++;
	board.whiteTurn = !board.whiteTurn;
	int max = -4096;
	for (size_t i = 0; i < moves.size(); i++)
	{
		ArrayBoard boardCopy = ArrayBoard(board);
		boardCopy.makeMove(moves[i]);
		int score = -negaMax(boardCopy, depth - 1, ply + 1, maxDepth, triangularPVTable, pvNextIndex);
		if (score > max)
		{
			triangularPVTable[pvIndex] = moves[i];
			movcpy(triangularPVTable + pvIndex + 1, triangularPVTable + pvNextIndex, maxDepth - ply - 1);
			max = score;
		}
	}
	return max;
}

int Engine::alphaBeta(ArrayBoard board, int alpha, int beta, short depth, short ply, short maxDepth, __int16 triangularPVTable[], short pvIndex)
{
	//Negamax with alpha beta
	nodes[depth]++;
	if (depth == 0)
		return board.whiteTurn ? evaluateBoard(board) : -evaluateBoard(board);
	triangularPVTable[pvIndex] = 0; //No principal variation found yet
	int pvNextIndex = pvIndex + maxDepth - ply;
	vector<__int16> moves;
	if (board.whiteTurn)
		moves = board.generateWhiteLegalMoves();
	else
		moves = board.generateBlackLegalMoves();
	board.totalPly++;
	board.whiteTurn = !board.whiteTurn;
	for (size_t i = 0; i < moves.size(); i++)
	{
		ArrayBoard boardCopy = ArrayBoard(board);
		boardCopy.makeMove(moves[i]);
		int score = -alphaBeta(boardCopy, -beta, -alpha, depth - 1, ply + 1, maxDepth, triangularPVTable, pvNextIndex);
		if (score >= beta)
		{
			return beta;
		}
		if (score > alpha)
		{
			alpha = score;
			triangularPVTable[pvIndex] = moves[i];
			movcpy(triangularPVTable + pvIndex + 1, triangularPVTable + pvNextIndex, maxDepth - ply - 1);
		}
	}
	return alpha;
}

vector<__int16> Engine::startSearch(ArrayBoard board, int timeLeft, int maxDepth)
{
	//Create a triangular table to keep track of Principal Variations
	__int16 *triangularPV = new __int16[(maxDepth*maxDepth + maxDepth) / 2];
	for (size_t i = 0; i < 7; i++)
	{
		nodes[i] = 0;
	}
	clock_t timer = clock();

	//Run search
	int score = alphaBeta(board, -4096, 4096, maxDepth, 0, maxDepth, triangularPV, 0);
	//int score = negaMax(board, maxDepth, 0, maxDepth, triangularPV, 0);

	double duration = (clock() - timer) / (double)CLOCKS_PER_SEC;
	cout << "Score " << score << " took " << to_string(duration) << " s at depth " << maxDepth << endl;
	
	for (size_t i = 7; i > 0; i--)
	{
		cout << "Nodes at ply " << to_string(5 - i) << ": " << to_string(nodes[i]) << endl;
	}
	//Grab pV from triangular array
	vector<__int16> pV;
	for (size_t i = 0; i < maxDepth; i++)
	{
		pV.push_back(triangularPV[i]);
		cout << IO::convertMoveToAlg(triangularPV[i]) << endl;
	}
	return pV;
}

Engine::Engine()
{
}


Engine::~Engine()
{
}
