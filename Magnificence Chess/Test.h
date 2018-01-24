#pragma once

#include <string>
#include "ArrayBoard.h"

class Test
	//Static class for testing functions
{
public:
	//Display
	static char pieceToChar(int piece);
	static string pieceToString(int piece);
	static string displayBoard(ArrayBoard board);

	//Perft
	static long recursivePerft(ArrayBoard board, int depth);

	static vector<string> perftDivide(ArrayBoard board, int depth);

private:
	Test();
	~Test();
};

