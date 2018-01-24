#pragma once
#include <string>
#include "Board.h"
#include "BitBoard.h"
#include "ArrayBoard.h"
#include <string>

using namespace std;

//IO takes care of converting between various formats, such as the Move class and
//long algebraic notation or the Board class and FEN strings.
//Static class, declaration is private to prevent creation of object. I like static classes, ok?
class IO
{
public:
	static string convertMoveToAlg(__int16 move);
	static __int16 convertAlgToMove(string alg);

	static string convertBoardToFEN(BitBoard board);
	static string convertBoardToFEN(ArrayBoard board);

	static BitBoard convertFENtoBitBoard(string fenString);
	static ArrayBoard convertFENtoArrayBoard(string fenString);
private:
	IO();
	~IO();
};

vector<string> split(string input, char delimiter);

