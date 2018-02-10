#pragma once
#include "ArrayBoard.h"
#include <vector>
#include "TranspositionTable.h"

class Engine
{
public:

	int evaluateBoard(ArrayBoard &board); //Evaluates the board

	int negaMax(ArrayBoard board, int depth, int ply, int maxDepth, __int16 triangularPVTable[], short pvIndex); //Main recursive search

	int alphaBeta(ArrayBoard board, int alpha, int beta, short depth, short ply, short maxDepth, __int16 triangularPVTable[], short pvIndex);

	int alphaBetaTT(ArrayBoard board, int alpha, int beta, short depth, short ply, short maxDepth, __int16 triangularPVTable[], short pvIndex, TranspositionTable * transpositionTable);

	vector<__int16> startSearch(ArrayBoard board, int timeLeft, int maxDepth);

	Engine();
	~Engine();
private:
	TranspositionTable transpositionTable = TranspositionTable();

	void movcpy(__int16* pTarget, const __int16* pSource, int n);
	//Transposition table
	//Values
	const short pawnValue = 100;
	const short knightValue = 300;
	const short bishopValue = 300;
	const short rookValue = 500;
	const short queenValue = 900;
};

