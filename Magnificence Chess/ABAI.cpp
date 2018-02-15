#include "stdafx.h"
#include "ABAI.h"
#pragma once
#include "IO.h"


ABAI::ABAI()
{
	FirstMoveObject = new MoveObject();
	FirstMoveObject->Moves = new u32[218];
	FirstMoveObject->nextObject = new MoveObject();
	FirstMoveObject->nextObject->Moves = new u32[218];
	FirstMoveObject->nextObject->nextObject = nullptr;
}


ABAI::~ABAI()
{
}

void ABAI::movcpy(u32* pTarget, const u32* pSource, int n)
{
	while (n-- && (*pTarget++ = *pSource++));
}

int ABAI::negamax(int alpha, int beta, int depth, int maxDepth, bool color, MoveObject *moveStor, u32 *triangularPV, short pvIndex)
{
	if (depth == 0)
	{
		if (color)
			return eval();
		else
			return -eval();
	}
	else
	{
		triangularPV[pvIndex] = 0;
		u16 pvNextIndex = pvIndex + depth;
		u32 *end, *start = moveStor->Moves;
		if (color)
			end = bb->WhiteLegalMoves(start);
		else
			end = bb->BlackLegalMoves(start);
		if (moveStor->nextObject == nullptr)
		{
			moveStor->nextObject = new MoveObject();
			moveStor->nextObject->Moves = new u32[218];
		}
		depth--;
		color = !color;
		while (start != end)
		{
			u32 move = *start;
			start++;
			if (move != 0 && move != 1)
			{
				bb->MakeMove(move);
				int returned = -negamax(-beta, -alpha, depth, maxDepth, color, moveStor->nextObject, triangularPV, pvNextIndex);
				bb->UnMakeMove(move);
				if (returned >= beta)
				{
					return beta;
				}
				if (returned > alpha)
				{
					triangularPV[pvIndex] = move;
					movcpy(triangularPV + pvIndex + 1, triangularPV + pvNextIndex, depth);
					alpha = returned;
				}
			}
		}
		return alpha;
	}
}

int ABAI::eval()
{
	nodes++;
	return (int)bb->pc(bb->Pieces[5]) * Pawn + bb->pc(bb->Pieces[4]) * Knight + bb->pc(bb->Pieces[3]) * Rook + bb->pc(bb->Pieces[2]) * Bishop + bb->pc(bb->Pieces[1]) * Queen
		- (int)(bb->pc(bb->Pieces[12]) * Pawn + bb->pc(bb->Pieces[11]) * Knight + bb->pc(bb->Pieces[10]) * Rook + bb->pc(bb->Pieces[9]) * Bishop + bb->pc(bb->Pieces[8]) * Queen);
}

vector<u32> ABAI::bestMove(BitBoard * IBB, bool color, clock_t time, int maxDepth)
{
	u32 *triangularPV = new u32[(maxDepth * (maxDepth + 1)) / 2];
	this->bb = IBB;
	nodes = 0;
	if (FirstMoveObject == nullptr)
	{
		FirstMoveObject = new MoveObject();
		FirstMoveObject->Moves = new u32[218];
		FirstMoveObject->nextObject = nullptr;
	}
	vector<u32> PV;
	u32 movePlaceHolder = 0;
	int score = negamax(-1000000, 1000000, maxDepth, maxDepth, color, FirstMoveObject, triangularPV, 0);
	cout << score << "   " << nodes << endl;
	for (size_t i = 0; i < maxDepth; i++)
	{
		PV.push_back(triangularPV[i]);
		cout << IO::convertMoveToAlg(triangularPV[i]) << endl;
	}
	delete[] triangularPV;
	return PV;
}
