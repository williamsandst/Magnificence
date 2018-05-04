#pragma once
#include <chrono>
#include "BitBoard.h"
#include <memory>
#include "GameState.h"
struct UnpackedHashEntry;
struct PackedHashEntry;
class Evaluation;

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
	u32 history[2][13][64];
	//Aspiration windows
	int aspirationAlpha = -8192;
	int aspirationBeta = 8192;

	//Hashtable size
	const int hashSizeBits = 24;

	//Arrays for moves and stuff
	u32 MoveArray[218 * 200];
	i32 sortArray[218 * 200];

	u64 nodes[100];
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
	void sortMoves(u32 *start, u32 *end, u32 bestMove, u16 *killerMoves, i32 *score, bool color);
	void sortQMoves(u32 *start, u32 *end, u16 *killerMoves, i32 *score);
	void fetchBest(u32 *start, u32 *end, i32 *score);
	u32 extractBestMove(PackedHashEntry in);
	u64 extractKey(PackedHashEntry in);
	u8 extractGeneration(PackedHashEntry in);
	int insertTT(PackedHashEntry newEntry);
	bool getFromTT(u64 key, UnpackedHashEntry *in);
	int qSearch(int alpha, int beta, bool color, u16 *killerMoves, u32 *start, i32 *score);
	int negamax(int alpha, int beta, int depth, int maxDepth, bool color, u32 *start, u16 *killerMoves, i32 * moveSortValues);
	int selfPlay(int depth, int moves, GameState *game);

	void resetTT();
	vector<u32> search(GameState &gameState);
	vector<u32> searchID(GameState &gameState);
	vector<u32> searchIDSimpleTime(GameState &gameState);
	vector<u32> searchIDComplexTime(GameState &gameState);

};

