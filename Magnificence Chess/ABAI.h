#pragma once
#include <chrono>
#include "BitBoard.h"
#include <memory>
struct UnpackedHashEntry;
struct PackedHashEntry;

struct UnpackedHashEntry
{
	u8 typeOfNode;//(0 is minimum value, 1 is exact, 2 is maximum value)
	u8 generation;
	short depth, score;
	u32 bestMove;
	u64 key;
	UnpackedHashEntry(u8 typeOfNode, short depth, short score, u32 bestM, u64 key, u8 generation);
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
public:
	const int Bishop = 300, Rook = 500, Knight = 300, Queen = 900, Pawn = 100;
	BitBoard *bb;
	PackedHashEntry *ttDepthFirst, *ttAlwaysOverwrite;
	u8 generation;
	u64 nodes, hashMask;
	ABAI();
	~ABAI();
	u8 extractNodeType(PackedHashEntry in);
	short extractDepth(PackedHashEntry in);
	short extractScore(PackedHashEntry in);
	u32 extractBestMove(PackedHashEntry in);
	u64 extractKey(PackedHashEntry in);
	u8 extractGeneration(PackedHashEntry in);
	void movcpy(u32* pTarget, const u32* pSource, int n);
	int insertTT(UnpackedHashEntry newEntry);
	bool getFromTT(u64 key, UnpackedHashEntry *in);
	int negamax(int alpha, int beta, int depth, int maxDepth, bool color, u32 *start, u32 *triangularPV, short pvIndex);
	int eval();
	vector<u32> bestMove(BitBoard *IBB, bool color, clock_t time, int maxDepth);

};

