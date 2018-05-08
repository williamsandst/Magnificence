#pragma once
#include <chrono>
#include "BitBoard.h"
#include <memory>
#include "GameState.h"
#include "TranspositionTable.h"

class Evaluation;

class ABAI
{
private:
	u32 history[2][13][64];
	//Aspiration windows
	int aspirationAlpha = -8192;
	int aspirationBeta = 8192;

	//Hashtable size
	const int hashSizeBits = 24;

	//Arrays for moves and stuff
	u32 MoveArray[218 * 200];
	i32 sortArray[218 * 200];
	u16 killerMoves[200];

	
public:
	bool *cont;
	u64 nodes[100];
	TranspositionTable *tt;
	BitBoard *bb;
	const u16 ToFromMask = 0b111111111111;
	u8 generation;
	u64 hashMask;
	ABAI();
	ABAI(TranspositionTable *tt);
	~ABAI();

	void sortMoves(u32 *start, u32 *end, u32 bestMove, u16 *killerMoves, i32 *score, bool color);
	void sortQMoves(u32 *start, u32 *end, u16 *killerMoves, i32 *score);
	void fetchBest(u32 *start, u32 *end, i32 *score);
	int qSearch(int alpha, int beta, bool color, u16 *killerMoves, u32 *start, i32 *score);
	int negamax(int alpha, int beta, int depth, int maxDepth, bool color, u32 *start, u16 *killerMoves, i32 * moveSortValues);
	int selfPlay(int depth, int moves, GameState *game);
	void resetNodes();
	void resetTT();
	int search(u8 depth, u8 generation, TranspositionTable *tt, BitBoard *bb, bool color);
	void divideHistory(int d);
};

