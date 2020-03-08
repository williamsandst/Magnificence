#pragma once
#include <vector>
#include <iostream>
#include <ctime>
#include "BitBoard.h"
#include "Move.h"
#include "Test.h"

class Engine
{
public:

	int evaluateBoard(BitBoard &board); //Evaluates the board

	int negaMax(BitBoard board, int depth, int ply, bool color, int maxDepth, u32 triangularPVTable[], short pvIndex, u32 * moveStart); //Main recursive search

	int negaMax2(BitBoard * board, int depth, bool color, u32 * moveStart);

	//int alphaBeta(BitBoard board, int alpha, int beta, short depth, short ply, short maxDepth, u32 triangularPVTable[], short pvIndex);


	vector<u32> startSearch(BitBoard * board, bool color, int timeLeft, int maxDepth);
	
	Engine();
	~Engine();
private:

	void movcpy(u32* pTarget, const u32* pSource, int n);
	//Transposition table
	//Values
	const short pawnValue = 100;
	const short knightValue = 300;
	const short bishopValue = 300;
	const short rookValue = 500;
	const short queenValue = 900;
};

