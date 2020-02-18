#include "stdafx.h"
#include "Engine.h"
#include "Move.h"
#include <iostream>
#include "IO.h"
#include <ctime>
#pragma once
#include "Test.h"

int nodes[100];
int transpositions = 0;

int Engine::evaluateBoard(BitBoard & board)
{
	return board.pc(board.Pieces[1])*queenValue + board.pc(board.Pieces[2])*bishopValue +
		board.pc(board.Pieces[3])*rookValue + board.pc(board.Pieces[4])*knightValue +
		board.pc(board.Pieces[5])*pawnValue -
		board.pc(board.Pieces[8])*queenValue - board.pc(board.Pieces[9])*bishopValue -
		board.pc(board.Pieces[10])*rookValue - board.pc(board.Pieces[11])*knightValue -
		board.pc(board.Pieces[12])*pawnValue;
}

void Engine::movcpy(u32* pTarget, const u32* pSource, int n) {
	while (n-- && (*pTarget++ = *pSource++));
}

int Engine::negaMax(BitBoard board, int depth, int ply, bool color, int maxDepth, u32 triangularPVTable[], short pvIndex, u32 *start)
{
	//Normal minmax
	nodes[depth]++;
	if (depth == 0)
		return 0;// color ? evaluateBoard(board) : -evaluateBoard(board);
	//triangularPVTable[pvIndex] = 0; //No principal variation found yet
	//int pvNextIndex = pvIndex + maxDepth - ply;
	u32 *end;
	if (color)
		end = board.WhiteLegalMoves(start);
	else
		end = board.BlackLegalMoves(start);
	color = !color;
	int max = -4096;

	u32 *nextStart = (start + 218);
	if (end - start == 1)
	{
		if (*start == 1 || *start == 0)
		{
			return -10000;
		}
	}
	while (start != end)
	{
		if (depth == maxDepth)
		{
			cout << "Did one!" << endl;
		}
		u32 move = *start;
		board.MakeMove(move);
		int score = -negaMax(board, depth - 1, color, ply + 1, maxDepth, triangularPVTable, 0, nextStart);
		board.UnMakeMove(move);
		if (score > max)
		{
			//triangularPVTable[pvIndex] = move;
			//movcpy(triangularPVTable + pvIndex + 1, triangularPVTable + pvNextIndex, maxDepth - ply - 1);
			max = score;
		}
		start++;
	}
	return max;
}

int Engine::negaMax2(BitBoard * board, int depth, bool color, u32 * start)
{
	nodes[depth]++;
	if (depth == 0)
		return color ? evaluateBoard(*board) : -evaluateBoard(*board);
	u32 *end;
	if (color)
		end = board->WhiteLegalMoves(start);
	else
	end = board->BlackLegalMoves(start);
	u32 *nextStart = (start + 218);
	int res = 0;
	depth--;
	color = !color;
	while (start != end)
	{
		u32 move = *start;
		start++;
		if (move != 0 && move != 1)
		{
			board->MakeMove(move);
			res += negaMax2(board, depth, color, nextStart);
			board->UnMakeMove(move);
		}
	}
	return res;
}

/*int Engine::alphaBeta(BitBoard board, int alpha, int beta, short depth, short ply, short maxDepth, __int16 triangularPVTable[], short pvIndex)
{
	//Negamax with alpha beta
	//nodes[depth]++;
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
}*/

vector<u32> Engine::startSearch(BitBoard * board, bool color, int timeLeft, int maxDepth)
{
	//Create a triangular table to keep track of Principal Variations
	u32 *triangularPV = new u32[(maxDepth*maxDepth + maxDepth) / 2];

	//Debug output reset
	for (size_t i = 0; i < 100; i++)
	{
		nodes[i] = 0;
	}
	clock_t timer = clock();

	u32 *start = new u32[218 * (maxDepth + 1)];

	//Run search
	//int score = alphaBeta(board, -4096, 4096, maxDepth, 0, maxDepth, triangularPV, 0);
	//int score = negaMax(*board, maxDepth, color, 0, maxDepth, triangularPV, 0, start);

	int score = negaMax2(board, maxDepth, color, start);

	//Debug output
	double duration = (clock() - timer) / (double)CLOCKS_PER_SEC;
	cout << "Score " << score << " took " << to_string(duration) << " s at depth " << maxDepth << endl;
	cout << "Total transpositions performed: " << to_string(transpositions) << endl;
	for (size_t i = 0; i < maxDepth; i++)
	{
		cout << "Nodes at ply " << to_string(i) << ": " << to_string(nodes[i]) << endl;
	}
	//Grab pV from triangular array
	vector<u32> pV;
	for (size_t i = 0; i < maxDepth; i++)
	{
		pV.push_back(triangularPV[i]);
		cout << IO::convertMoveToAlg(triangularPV[i]) << endl;
	}
	delete[] start;
	return pV;
}

Engine::Engine()
{
}


Engine::~Engine()
{
}
