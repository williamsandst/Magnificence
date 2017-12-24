#pragma once
#include <string>
#include "Board.h"

using namespace std;

//IO takes care of converting between various formats, such as the Move class and
//long algebraic notation or the Board class and FEN strings.
//Static class, declaration is private to prevent creation of object. I like static classes, ok?
class IO
{
public:
	string convertMoveToAlg();
	void convertAlgToMove();
	string convertBoardToFEN();
	Board convertFENtoBoard();
private:
	IO();
	~IO();
};

