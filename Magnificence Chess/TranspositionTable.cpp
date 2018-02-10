#include "stdafx.h"
#include "TranspositionTable.h"
#include <iostream>


void TranspositionTable::generateRandomNumbers(int seed)
{
	mt19937_64 mtRand(seed);
	for (size_t i = 0; i < 17*64; i++) //Pieces
	{
		zPieces[i] = mtRand();
	}
	for (size_t i = 0; i < 2; i++) //Castling
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

HashEntry & TranspositionTable::getHashObject(int i)
{
	return hashTable[i];
}

int TranspositionTable::findHashTableMatch(unsigned long long zobrist)
{
	int index = zobrist % hashTableSize;
	if (hashTable[index].zobristKey == zobrist)
		return index;
	return -1;
}

void TranspositionTable::addHash(unsigned long long zobrist, int depth, int score, int nodeType)
{
	int index = zobrist % hashTableSize;
	if (hashTable[index].zobristKey == 0) //Nothing here, add the variables
	{
		hashTable[index].zobristKey = zobrist;
		hashTable[index].depth = depth;
		hashTable[index].score = score;
		hashTable[index].nodeType = nodeType;
	}
	/*else if (hashTable[index].zobristKey == zobrist)
	{
		if (depth > hashTable[index].depth)
		{
			hashTable[index].zobristKey = zobrist;
			hashTable[index].depth = depth;
			hashTable[index].score = score;
			hashTable[index].nodeType = nodeType;
		}
	}*/
}

unsigned long TranspositionTable::getZobristKey(ArrayBoard const *board)
{
	unsigned long long zobristKey = 0;
	for (size_t i = 0; i < 64; i++)
	{
		if (board->board[i] != 0)
			zobristKey ^= zPieces[64*(board->board[i]-1) + i];
	}
	if (board->castlingBlackKingSide) zobristKey ^= zCastlingBlack[0];
	if (board->castlingBlackQueenSide) zobristKey ^= zCastlingBlack[1];
	if (board->castlingWhiteKingSide) zobristKey ^= zCastlingWhite[0];
	if (board->castlingWhiteQueenSide) zobristKey ^= zCastlingWhite[1];
	if (board->enPassantSquare > 0) zobristKey ^= zEnPassant[board->enPassantSquare];
	if (board->whiteTurn) zobristKey ^= zSide;
	return zobristKey;
}

unsigned long TranspositionTable::getZobristKeySwitchSide(ArrayBoard const *board)
{
	unsigned long long zobristKey = 0;
	for (size_t i = 0; i < 64; i++)
	{
		if (board->board[i] != 0)
			zobristKey ^= zPieces[64 * (board->board[i] - 1) + i];
	}
	if (board->castlingBlackKingSide) zobristKey ^= zCastlingBlack[0];
	if (board->castlingBlackQueenSide) zobristKey ^= zCastlingBlack[1];
	if (board->castlingWhiteKingSide) zobristKey ^= zCastlingWhite[0];
	if (board->castlingWhiteQueenSide) zobristKey ^= zCastlingWhite[1];
	if (board->enPassantSquare > 0) zobristKey ^= zEnPassant[board->enPassantSquare];
	if (board->whiteTurn) zobristKey ^= zSide;
	return zobristKey;
}

unsigned long TranspositionTable::updateZobristKey(unsigned long long zobristKey, int from, int fromPiece, int to, int toPiece)
{
	//Odd behaviour with zero maybe? All zero positions have the same value
	//Remove from-piece
	zobristKey ^= zPieces[64*(fromPiece-1) + from];
	//Remove to-piece
	if (toPiece != 0)
		zobristKey ^= zPieces[64 * (toPiece - 1) + to];
	//Add to-piece
	zobristKey ^= zPieces[64 * (fromPiece - 1) + to];
	//Switch side
	zobristKey ^= zSide;
	return zobristKey;
}


//En passant move

TranspositionTable::TranspositionTable()
{
	generateRandomNumbers(1234567890);
}


TranspositionTable::~TranspositionTable()
{
}

HashEntry::HashEntry()
{
}

HashEntry::~HashEntry()
{
}
