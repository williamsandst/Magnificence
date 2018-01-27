#pragma once

#include "ArrayBoard.h"

class TranspositionTable
{
private:
	long long zPieces[16 * 64];   // [piece type][side to move][square]
	long long zCastlingWhite[4]; // Four different castling setups
	long long zCastlingBlack[4]; // both ways, short, long and none
	long long zEnPassant[64];      // En passant
	long long zSide;                 // Used for changing sides
	void generateRandomNumbers(int seed);
public:
	long getZobristKey(ArrayBoard board)
	{
		unsigned long long int zobrist;
	}
	TranspositionTable();
	~TranspositionTable();
};

