#pragma once
#include "Board.h"

using namespace std;

class ArrayBoard : public Board
{
public:
	__int8 board[64] = { };
	bool castlingWhiteQueenSide = true;
	bool castlingWhiteKingSide = true;
	bool castlingBlackQueenSide = true;
	bool castlingBlackKingSide = true;
	unsigned __int8 totalPly = 1;
	unsigned __int8 drawCounter = 0;
	__int8 enPassantSquare = -1;
	ArrayBoard(string fenString);
	void makeMove(__int16 move);
	void undoMove(__int16 move);
	//Enpassant
	ArrayBoard();
	~ArrayBoard();
};

