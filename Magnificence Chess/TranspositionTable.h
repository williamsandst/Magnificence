#pragma once
#include "BitBoard.h"
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
class TranspositionTable
{
private:
	PackedHashEntry * tt;
	u32 hashMask;
	u8 hashSizeBits;

public:
	u8 generation;
	short extractDepth(PackedHashEntry in);
	short extractScore(PackedHashEntry in);
	u8 extractNodeType(PackedHashEntry in);
	u32 extractBestMove(PackedHashEntry in);
	u64 extractKey(PackedHashEntry in);
	u8 extractGeneration(PackedHashEntry in);
	void resetTT();
	int insertTT(PackedHashEntry newEntry);
	bool getFromTT(u64 key, UnpackedHashEntry *in);
	TranspositionTable();
	TranspositionTable(u8 hashSizeBits);
	~TranspositionTable();
	void setHashSizeBits(u8 bits);
};

