#pragma once
#include "Move.h"
class ArrayBoard;
#include "ArrayBoard.h"
#include <random>

struct HashEntry //136 bits, 17 bytes?
{
public:
	unsigned long long zobristKey = 0;
	__int16 bestMove;
	short depth;
	short score;
	short nodeType; //-1,0,1
	bool ancient;
	HashEntry();
	~HashEntry();
};

class TranspositionTable
{
private:
	//Random numbers
	unsigned long long zPieces[16 * 64];   // [piece type][side to move][square]
	unsigned long long zCastlingWhite[2]; // Four different castling setups
	unsigned long long zCastlingBlack[2]; // both ways, short, long and none
	unsigned long long zEnPassant[64];      // En passant
	unsigned long long zSide;                 // Used for changing sides

	const int hashTableSize = 1024;
	//Transposition hashtable
	HashEntry hashTable[1024] = {HashEntry()};
	
	void generateRandomNumbers(int seed);
public:
	HashEntry & getHashObject(int i);
	//Look for matching hash
	int findHashTableMatch(unsigned long long zobrist);

	void addHash(unsigned long long zobrist, int depth, int score, int evalType);

	//MakeMove
	unsigned long getZobristKey(ArrayBoard const *board);

	unsigned long getZobristKeySwitchSide(ArrayBoard const * board);

	unsigned long updateZobristKey(unsigned long long zobristKey, int from, int fromPiece, int to, int toPiece);
	unsigned long updateZobristKeyPromotion(__int16 move, bool side);
	unsigned long updateZobristKeyEPMove(__int16 move, bool side);
	unsigned long updateZobristKeyEP(long long zobrist, __int16 enPassant);
	unsigned long updateZobristKeyCastling(long long zobrist, __int16 move, bool wq, bool wk, bool bq, bool bk);
	
	
	TranspositionTable();
	~TranspositionTable();
};