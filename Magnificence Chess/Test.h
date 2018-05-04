#pragma once
#include <string>
#include "BitBoard.h"
#include "GameState.h"
#include "Engines.h"

struct HashEntryPerft
{
private:
	u64 key, Result;
public:
	HashEntryPerft(u64 key, u64 result, u8 depth)
	{
		result = (result << 8) | depth;
		this->key = key ^ result;
		this->Result = result;
	}
	HashEntryPerft()
	{
		this->key = 0;
		this->Result = 0;
	}
	u64 GetKey()
	{
		return (this->key ^ this->Result);
	}
	u64 GetResult()
	{
		return ((this->Result >> 8));
	}
	u8 GetDepth()
	{
		return (u8)(this->Result & 0b11111111);
	}
};

class Test
	//Static class for testing functions
{
public:
	//Display
	static char pieceToChar(int piece);
	static string pieceToString(int piece);
	static string displayBoard(BitBoard board);

	//Perft
	static u64 perftLazySMP(int depth, int startDepth, BitBoard *bb, bool color, u32 *start, HashEntryPerft *Hash, u32 tableSize);
	static u64 perftHash(int depth, int startDepth, BitBoard *bb, bool color, u32 *start, HashEntryPerft *Hash, u32 tableSize, bool *output);
	static u64 perft(int depth, BitBoard *bb, bool color, u32 *start);
	static string perftDivide(int depth, BitBoard *bb, bool color, u32 *start);

	//Testsuites
	static int LCT2(GameState *gameState, bool timeSearch);

	//Debugging BitBoard
	//static bool tester(int depth, BitBoard *bb, bool color, int startDepth);
	static void seePos(BitBoard *bb);
	static bool hasBeenCorrupted(BitBoard *bb);

private:
	Test();
	~Test();
};

