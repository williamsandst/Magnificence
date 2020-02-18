#pragma once
#include <string>
#include "Board.h"
#include "BitBoard.h"

using namespace std;

//IO takes care of converting between various formats, such as the Move class and
//long algebraic notation or the Board class and FEN strings.
//Static class, declaration is private to prevent creation of object. I like static classes, ok?
class IO
{
public:
	static string convertMoveToAlg(u32 move);
	static u32 convertAlgToMove(string alg);

	static string convertBoardToFEN(BitBoard board, bool color);
	//Returns mailbox as pointer
	static int * convertFENtoBoard(string fenString);

	static string displayBoard(BitBoard board);
private:
	IO();
	~IO();
};

vector<string> split(string input, char delimiter);

