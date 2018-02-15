#pragma once
#include <chrono>
#include "BitBoard.h"
#include <memory>

struct MoveObject
{
	u32 *Moves;
	MoveObject *nextObject;
};

class ABAI
{
public:
	const int Bishop = 300, Rook = 500, Knight = 300, Queen = 900, Pawn = 100;
	MoveObject *FirstMoveObject;
	BitBoard *bb;
	u64 nodes;
	ABAI();
	~ABAI();
	void movcpy(u32* pTarget, const u32* pSource, int n);
	int negamax(int alpha, int beta, int depth, int maxDepth, bool color, MoveObject *stor, u32 *triangularPV, short pvIndex);
	int eval();
	vector<u32> bestMove(BitBoard *IBB, bool color, clock_t time, int maxDepth);

};

