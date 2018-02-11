#pragma once
#include <string>
#include "BitBoard.h"

class Test
	//Static class for testing functions
{
public:
	//Display
	static char pieceToChar(int piece);
	static string pieceToString(int piece);
	static string displayBoard(BitBoard board);

	//Perft
	static u64 perft(int depth, BitBoard *bb, bool color);
	static string perftDivide(int depth, BitBoard *bb, bool color);

	//Debugging BitBoard
	static bool tester(int depth, BitBoard *bb, bool color, int startDepth);
	static void seePos(BitBoard *bb);
	static bool hasBeenCorrupted(BitBoard *bb);

private:
	Test();
	~Test();
};

