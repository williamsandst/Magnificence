#pragma once
#include "Board.h"
#include <set>
#include <vector>

using namespace std;

class ArrayBoard : public Board
{
public:
	__int8 board[64] = { };
	bool castlingWhiteQueenSide = true;
	bool castlingWhiteKingSide = true;
	bool castlingBlackQueenSide = true;
	bool castlingBlackKingSide = true;
	bool whiteTurn = true;
	unsigned __int8 totalPly = 1;
	unsigned __int8 drawCounter = 0;
	__int8 enPassantSquare = -1;
	ArrayBoard(string fenString);
	void makeMoveBlack(__int16 move);
	void makeMoveWhite(__int16 move);
	void makeMove(__int16 move);
	void undoMove(__int16 move);
	vector<__int16> generateWhiteLegalMoves();
	vector<__int16> generateBlackLegalMoves();

	bool squareAttackedByBlack(int piecePos);
	bool squareAttackedByWhite(int piecePos);

	vector<__int16> generateWhiteEvasionMoves(vector<__int16> &moves, int attackingPos, int kingPos, vector<short> &pinnedPiecesSet);
	
	vector<__int16> generateBlackEvasionMoves(vector<__int16> &moves, int attackingPos, int kingPos, vector<short> &pinnedPiecesSet);

	void whiteBlockingMoves(vector<__int16> &moves, int index, vector<short> &pinnedPiecesSet);
	void blackBlockingMoves(vector<__int16> &moves, int index, vector<short> &pinnedPiecesSet);

	//Enpassant
	ArrayBoard();
	ArrayBoard(const ArrayBoard &copy);
	~ArrayBoard();
};

