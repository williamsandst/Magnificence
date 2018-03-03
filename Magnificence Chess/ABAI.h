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
	unsigned short depth;
	short score;
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
private:
	//Material values
	const int Bishop = 300, Rook = 500, Knight = 300, Queen = 900, Pawn = 100;
	//Piece square tables
	//Debugging variables
	u64 nodes[100];
	u8 moveOveride[100];
	u16 killerMoves[200];
public:
	BitBoard *bb;
	const u16 ToFromMask = 0b111111111111;
	PackedHashEntry *ttDepthFirst, *ttAlwaysOverwrite;
	u8 generation;
	u64 hashMask;
	ABAI();
	~ABAI();
	u8 extractNodeType(PackedHashEntry in);
	short extractDepth(PackedHashEntry in);
	short extractScore(PackedHashEntry in);
	void SortMoves(u32 *start, u32 *end, u32 bestMove, u16 *killerMoves);
	u32 extractBestMove(PackedHashEntry in);
	u64 extractKey(PackedHashEntry in);
	u8 extractGeneration(PackedHashEntry in);
	void movcpy(u32* pTarget, const u32* pSource, int n);
	int insertTT(PackedHashEntry newEntry);
	bool getFromTT(u64 key, UnpackedHashEntry *in);
	int negamax(int alpha, int beta, int depth, int maxDepth, bool color, u32 *start, u16 *killerMoves);
	int lazyEval();
	vector<u32> bestMove(BitBoard *IBB, bool color, clock_t time, int maxDepth);

};

