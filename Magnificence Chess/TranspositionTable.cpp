#include "stdafx.h"
#include "TranspositionTable.h"
#include <random>


void TranspositionTable::generateRandomNumbers(int seed)
{
	mt19937_64 mtRand(seed);
	for (size_t i = 0; i < 16*64; i++) //Pieces
	{
		zPieces[i] = mtRand();
	}
	for (size_t i = 0; i < 4; i++) //Castling
	{
		zCastlingBlack[i] = mtRand();
		zCastlingWhite[i] = mtRand();
	}
	for (size_t i = 0; i < 64; i++) //En passant
	{
		zEnPassant[i] = mtRand();
	}
	zSide = mtRand();
}

TranspositionTable::TranspositionTable()
{

}


TranspositionTable::~TranspositionTable()
{
}
