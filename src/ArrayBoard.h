#pragma once
#include "Board.h"
#include "IO.h"

using namespace std;

typedef unsigned long long int u64;
typedef unsigned long int u32;
typedef unsigned short int u16;
typedef signed short int i16;
typedef signed short int i8;
typedef unsigned char u8;

class ArrayBoard : public Board
{
public:
	i8 board[64] = { };
	bool castlingWhiteQueenSide = true;
	bool castlingWhiteKingSide = true;
	bool castlingBlackQueenSide = true;
	bool castlingBlackKingSide = true;
	u8 totalPly = 1;
	u8 drawCounter = 0;
	i8 enPassantSquare = -1;
	ArrayBoard(string fenString);
	void makeMove(u16 move);
	void undoMove(u16 move);
	//Enpassant
	ArrayBoard();
	~ArrayBoard();
};

