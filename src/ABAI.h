#pragma once
#define INTRINSICS_H_INCLUDED

#if defined(_WIN32)
#include <intrin.h>
#elif defined(__gnu_linux__) || defined(__linux__) || defined(__CYGWIN__)
#include "x86intrin.h"
#endif


#include <chrono>
#include <cmath>
#include <memory>

#include "BitBoard.h"
#include "GameState.h"
#include "IO.h"
#include "GameState.h"

struct UnpackedHashEntry;
struct PackedHashEntry;

struct UnpackedHashEntry
{
	u8 typeOfNode;//(0 is minimum value, 1 is exact, 2 is maximum value)
	u8 generation;
	unsigned short depth;
	short score;
	u32 bestMove;
	u64 key;
	UnpackedHashEntry(u8 typeOfNode, u16 depth, short score, u32 bestM, u64 key, u8 generation);
	UnpackedHashEntry(PackedHashEntry in);
};

struct PackedHashEntry
{
	u64 key, data;
	PackedHashEntry(UnpackedHashEntry start);
	PackedHashEntry();
};

class ABAI
{
private:
	const int hashSizeBits = 24;
	//Arrays for moves and stuff
	u32 MoveArray[218 * 200];
	i16 sortArray[218 * 200];
	//Material values
	const int Bishop = 300, Rook = 500, Knight = 300, Queen = 900, Pawn = 100;
	//Piece square tables

	//Bishop PST
	const short whiteBishopEarlyPST[64] =
	{ 19, 16, 17, 18, 18, 17, 16, 19,
	 -14, 23, 20, 21, 21, 20, 23,-14,
	  17, 20, 26, 23, 23, 26, 20, 17,
	  18, 21, 23, 28, 28, 23, 21, 18,
	  18, 21, 23, 28, 28, 23, 21, 18,
	  17, 20, 26, 23, 23, 26, 20, 17,
	  16, 23, 20, 21, 21, 20, 23, 16,
	  9,  6,  7,  8,  8,  7,  6,  9 };

	const short blackBishopEarlyPST[64] =
	{ 9,  6,  7,  8,  8,  7,  6,  9,
	 16, 23, 20, 21, 21, 20, 23, 16,
	 17, 20, 26, 23, 23, 26, 20, 17,
	 18, 21, 23, 28, 28, 23, 21, 18,
	 18, 21, 23, 28, 28, 23, 21, 18,
	 17, 20, 26, 23, 23, 26, 20, 17,
	-14, 23, 20, 21, 21, 20, 23,-14,
	 19, 16, 17, 18, 18, 17, 16, 19, };
			
	const short whiteBishopLatePST[64] =
	{ 20, 22, 24, 26, 26, 24, 22, 20,
	  22, 28, 30, 32, 32, 30, 28, 22,
	  24, 30, 34, 36, 36, 34, 30, 24,
	  26, 32, 36, 38, 38, 36, 32, 26,
	  26, 32, 36, 38, 38, 36, 32, 26,
	  24, 30, 34, 36, 36, 34, 30, 24,
	  22, 28, 30, 32, 32, 30, 28, 22,
	  20, 22, 24, 26, 26, 24, 22, 20 };

	const short blackBishopLatePST[64] =
	{ 20, 22, 24, 26, 26, 24, 22, 20,
	  22, 28, 30, 32, 32, 30, 28, 22,
	  24, 30, 34, 36, 36, 34, 30, 24,
	  26, 32, 36, 38, 38, 36, 32, 26,
	  26, 32, 36, 38, 38, 36, 32, 26,
	  24, 30, 34, 36, 36, 34, 30, 24,
	  22, 28, 30, 32, 32, 30, 28, 22,
	  20, 22, 24, 26, 26, 24, 22, 20 };

	const short whitePawnEarlyPST[64] = 
	  { 0,   5,  10,  15,  15,  10,   5,   0,
		2,   7,  12,  -5,  -5,  12,   7,   2,
		0,   5,  10,  15,  15,  10,   5,   0,
		0,   5,  10,  35,  35,  10,   5,   0,
		0,   5,  10,  25,  25,  10,   5,   0,
		0,   5,  10,  15,  15,  10,   5,   0,
		0,   5,  10,  15,  15,  10,   5,   0,
		0,   5,  10,  15,  15,  10,   5,   0, };

	const short blackPawnEarlyPST[64] = 
	  { 0,   5,  10,  15,  15,  10,   5,   0,
		0,   5,  10,  15,  15,  10,   5,   0,
		0,   5,  10,  15,  15,  10,   5,   0,
		0,   5,  10,  25,  25,  10,   5,   0,
		0,   5,  10,  35,  35,  10,   5,   0,
		0,   5,  10,  15,  15,  10,   5,   0,
		2,   7,  12,  -5,  -5,  12,   7,   2,
		0,   5,  10,  15,  15,  10,   5,   0, };
			
	const short whitePawnLatePST[64] = 
	  { 0,  0,  0,  0,  0,  0,  0,  0,
		0,  0,  0,  0,  0,  0,  0,  0,
		0,  0,  0,  0,  0,  0,  0,  0,
		0,  0,  0,  0,  0,  0,  0,  0,
		0,  0,  0,  0,  0,  0,  0,  0,
		0,  0,  0,  0,  0,  0,  0,  0,
		0,  0,  0,  0,  0,  0,  0,  0,
		0,  0,  0,  0,  0,  0,  0,  0 };

	const short blackPawnLatePST[64] = 
	  { 0,  0,  0,  0,  0,  0,  0,  0,
		0,  0,  0,  0,  0,  0,  0,  0,
		0,  0,  0,  0,  0,  0,  0,  0,
		0,  0,  0,  0,  0,  0,  0,  0,
		0,  0,  0,  0,  0,  0,  0,  0,
		0,  0,  0,  0,  0,  0,  0,  0,
		0,  0,  0,  0,  0,  0,  0,  0,
		0,  0,  0,  0,  0,  0,  0,  0 };

	const short whiteKnightEarlyPST[64] = 
	{ -15, -12,  -9,  -6,  -6,  -9, -12, -15,
		3,  12,  15,  18,  18,  15,  12,   3,
		6,  15,  21,  24,  24,  21,  15,   6,
		9,  18,  24,  27,  27,  24,  18,   9,
		9,  18,  27,  32,  32,  27,  18,   9,
		6,  15,  21,  27,  27,  21,  15,   6,
		3,  12,  15,  18,  18,  15,  12,   3,
		-50,   3,   6,   9,   9,   6,   3, -50 };

	const short blackKnightEarlyPST[64] = 
	{ -50,   3,   6,   9,   9,   6,   3, -50,
		3,  12,  15,  18,  18,  15,  12,   3,
		6,  15,  21,  27,  27,  21,  15,   6,
		9,  18,  27,  32,  32,  27,  18,   9,
		9,  18,  24,  27,  27,  24,  18,   9,
		6,  15,  21,  24,  24,  21,  15,   6,
		3,  12,  15,  18,  18,  15,  12,   3,
	  -15, -12,  -9,  -6,  -6,  -9, -12, -15 };
			
	const short whiteKnightLatePST[64] = 
	  { 0,  3,  6,  9,  9,  6,  3,  0,
		3, 12, 15, 18, 18, 15, 12,  3,
		6, 15, 21, 24, 24, 21, 15,  6,
		9, 18, 24, 27, 27, 24, 18,  9,
		9, 18, 24, 27, 27, 24, 18,  9,
		6, 15, 21, 24, 24, 21, 15,  6,
		3, 12, 15, 18, 18, 15, 12,  3,
		0,  3,  6,  9,  9,  6,  3,  0, };

	const short blackKnightLatePST[64] =
      { 0,  3,  6,  9,  9,  6,  3,  0,
		3, 12, 15, 18, 18, 15, 12,  3,
		6, 15, 21, 24, 24, 21, 15,  6,
		9, 18, 24, 27, 27, 24, 18,  9,
		9, 18, 24, 27, 27, 24, 18,  9,
		6, 15, 21, 24, 24, 21, 15,  6,
		3, 12, 15, 18, 18, 15, 12,  3,
		0,  3,  6,  9,  9,  6,  3,  0, };

	const short whiteRookEarlyPST[64] =
	  { 0,  3,  6,  9,  9,  6,  3,  0,
		25, 28, 31, 34, 34, 31, 28, 25,
		0,  3,  6,  9,  9,  6,  3,  0,
		0,  3,  6,  9,  9,  6,  3,  0,
		0,  3,  6,  9,  9,  6,  3,  0,
		0,  3,  6,  9,  9,  6,  3,  0,
		0,  3,  6,  9,  9,  6,  3,  0,
		1,  4,  7, 10, 10,  7,  4,  1, };

	const short blackRookEarlyPST[64] = 
	  { 1,  4,  7, 10, 10,  7,  4,  1,
		0,  3,  6,  9,  9,  6,  3,  0,
		0,  3,  6,  9,  9,  6,  3,  0,
		0,  3,  6,  9,  9,  6,  3,  0,
		0,  3,  6,  9,  9,  6,  3,  0,
		0,  3,  6,  9,  9,  6,  3,  0,
		25, 28, 31, 34, 34, 31, 28, 25,
		0,  3,  6,  9,  9,  6,  3,  0};
			
	const short whiteRookLatePST[64] =
	  { 25, 25, 25, 25, 25, 25, 25, 25,
		25, 25, 25, 25, 25, 25, 25, 25,
		25, 25, 25, 25, 25, 25, 25, 25,
		25, 25, 25, 25, 25, 25, 25, 25,
		25, 25, 25, 25, 25, 25, 25, 25,
		25, 25, 25, 25, 25, 25, 25, 25,
		25, 25, 25, 25, 25, 25, 25, 25,
		25, 25, 25, 25, 25, 25, 25, 25, };

	const short blackRookLatePST[64] = 
	  { 25, 25, 25, 25, 25, 25, 25, 25,
		25, 25, 25, 25, 25, 25, 25, 25,
		25, 25, 25, 25, 25, 25, 25, 25,
		25, 25, 25, 25, 25, 25, 25, 25,
		25, 25, 25, 25, 25, 25, 25, 25,
		25, 25, 25, 25, 25, 25, 25, 25,
		25, 25, 25, 25, 25, 25, 25, 25,
		25, 25, 25, 25, 25, 25, 25, 25, };

	const short whiteQueenEarlyPST[64] = 
	  { 100,100,100,100,100,100,100,100,
		115,115,115,115,115,115,115,115,
		100,100,100,100,100,100,100,100,
		100,100,100,100,100,100,100,100,
		100,100,100,100,100,100,100,100,
		100,100,100,100,100,100,100,100,
		100,100,100,100,100,100,100,100,
		95, 95, 95, 95, 95, 95, 95, 95 };

	const short blackQueenEarlyPST[64] = 
	  { 95, 95, 95, 95, 95, 95, 95, 95,
		100,100,100,100,100,100,100,100,
		100,100,100,100,100,100,100,100,
		100,100,100,100,100,100,100,100,
		100,100,100,100,100,100,100,100,
		100,100,100,100,100,100,100,100,
		115,115,115,115,115,115,115,115,
		100,100,100,100,100,100,100,100, };
			
	const short whiteQueenLatePST[64] = 
	  { 80, 83, 86, 89, 89, 86, 83, 80,
		83, 92, 95, 98, 98, 95, 92, 83,
		86, 95,101,104,104,101, 95, 86,
		89, 98,104,107,107,104, 98, 89,
		89, 98,104,107,107,104, 98, 89,
		86, 95,101,104,104,101, 95, 86,
		83, 92, 95, 98, 98, 95, 92, 83,
		80, 83, 86, 89, 89, 86, 83, 80 };

	const short blackQueenLatePST[64] = 
	  { 80, 83, 86, 89, 89, 86, 83, 80,
		83, 92, 95, 98, 98, 95, 92, 83,
		86, 95,101,104,104,101, 95, 86,
		89, 98,104,107,107,104, 98, 89,
		89, 98,104,107,107,104, 98, 89,
		86, 95,101,104,104,101, 95, 86,
		83, 92, 95, 98, 98, 95, 92, 83,
		80, 83, 86, 89, 89, 86, 83, 80 };

	const short whiteKingEarlyPST[64] = 
	  { -175,-175,-175,-175,-175,-175,-175,-175,
		-150,-150,-150,-150,-150,-150,-150,-150,
		-125,-125,-125,-125,-125,-125,-125,-125,
		-100,-100,-100,-100,-100,-100,-100,-100,
		-75, -75, -75, -75, -75, -75, -75, -75,
		-50, -50, -50, -50, -50, -50, -50, -50,
		50,  50,   0,   0,   0,   0,  50,  50,
		50,  50,   0,   0,  20,   0,  50,  50 };

	const short blackKingEarlyPST[64] = 
	  { 50,  50,   0,   0,  20,   0,  50,  50,
		50,  50,   0,   0,   0,   0,  50,  50,
		-50, -50, -50, -50, -50, -50, -50, -50,
		-75, -75, -75, -75, -75, -75, -75, -75,
		-100,-100,-100,-100,-100,-100,-100,-100,
		-125,-125,-125,-125,-125,-125,-125,-125,
		-150,-150,-150,-150,-150,-150,-150,-150,
		-175,-175,-175,-175,-175,-175,-175,-175 };
			
	const short whiteKingLatePST[64] = 
	  { 0, 10, 20, 30, 30, 20, 10,  0,
		10, 40, 50, 60, 60, 50, 40, 10,
		20, 50, 70, 80, 80, 70, 50, 20,
		30, 60, 80, 90, 90, 80, 60, 30,
		30, 60, 80, 90, 90, 80, 60, 30,
		20, 50, 70, 80, 80, 70, 50, 20,
		10, 40, 50, 60, 60, 50, 40, 10,
		0, 10, 20, 30, 30, 20, 10,  0 };

	const short blackKingLatePST[64] = 
	  { 0, 10, 20, 30, 30, 20, 10,  0,
		10, 40, 50, 60, 60, 50, 40, 10,
		20, 50, 70, 80, 80, 70, 50, 20,
		30, 60, 80, 90, 90, 80, 60, 30,
		30, 60, 80, 90, 90, 80, 60, 30,
		20, 50, 70, 80, 80, 70, 50, 20,
		10, 40, 50, 60, 60, 50, 40, 10,
		0, 10, 20, 30, 30, 20, 10,  0  };
	
	u64 nodes[100];
	//u64 history[100];

	inline u32 bitScanForward(u64 piece)
	{
		#if defined(_WIN32)
		u32 index;
		_BitScanForward64(&index, piece);
		return index;
		#elif defined(__gnu_linux__) || defined(__linux__) || defined(__CYGWIN__)
		return __builtin_ctz(piece);
		#endif
	}

public:
	BitBoard *bb;
	const u16 ToFromMask = 0b111111111111;
	PackedHashEntry *ttDepthFirst=nullptr, *ttAlwaysOverwrite=nullptr;
	u8 generation;
	u64 hashMask;
	ABAI();
	~ABAI();
	u8 extractNodeType(PackedHashEntry in);
	short getPieceValue(u8 piece);
	short extractDepth(PackedHashEntry in);
	short extractScore(PackedHashEntry in);
	void sortMoves(u32 *start, u32 *end, u32 bestMove, u16 *killerMoves, i16 *score);
	void sortQMoves(u32 *start, u32 *end, u16 *killerMoves, i16 *score);
	void fetchBest(u32 *start, u32 *end, i16 *score);
	u32 extractBestMove(PackedHashEntry in);
	u64 extractKey(PackedHashEntry in);
	u8 extractGeneration(PackedHashEntry in);
	int insertTT(PackedHashEntry newEntry);
	bool getFromTT(u64 key, UnpackedHashEntry *in);
	int qSearch(int alpha, int beta, bool color, u16 *killerMoves, u32 *start, i16 *score);
	int negamax(int alpha, int beta, int depth, int maxDepth, bool color, u32 *start, u16 *killerMoves, i16 * moveSortValues);
	int selfPlay(int depth, int moves, GameState *game);

	int lazyEval();
	int pieceSquareValues(const short * pieceSquareTable, u64 pieceSet);
	void resetTT();
	vector<u32> bestMove(GameState &gameState);
	vector<u32> search(GameState &gameState);
	vector<u32> searchID(GameState &gameState);
	vector<u32> searchIDSimpleTime(GameState &gameState);
	vector<u32> searchIDComplexTime(GameState &gameState);

};

