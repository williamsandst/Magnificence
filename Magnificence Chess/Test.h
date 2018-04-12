#pragma once
#include <string>
#include "BitBoard.h"

struct HashEntryPerft
{
	u64 key, Result;
	u8 depth;
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
	static u64 perftHash(int depth, int startDepth, BitBoard *bb, bool color, u32 *start, HashEntryPerft *Hash, u32 tableSize);
	static u64 perft(int depth, BitBoard *bb, bool color, u32 *start);
	static string perftDivide(int depth, BitBoard *bb, bool color, u32 *start);

	//Debugging BitBoard
	//static bool tester(int depth, BitBoard *bb, bool color, int startDepth);
	static void seePos(BitBoard *bb);
	static bool hasBeenCorrupted(BitBoard *bb);

private:
	Test();
	~Test();
};

