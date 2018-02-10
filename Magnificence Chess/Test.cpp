#include "stdafx.h"
#include "Test.h"
#include "Move.h"
#include "IO.h"
#include <ctime>
#include "stdafx.h"
#include <algorithm>

//Static class used for testing purposes

string Test::displayBoard(ArrayBoard board)
{
	string boardString = "\n";
	for (size_t y = 0; y < 8; y++)
	{
		for (size_t x = 0; x < 8; x++)
		{
			boardString += pieceToChar(board.board[y * 8 + x]);
		}
		boardString += "\n";
	}
	boardString += "\n";
	return boardString;

}



long Test::recursivePerft(ArrayBoard board, int depth)
{
	long nodes = 0;
	//if (depth == 0) return 1;
	vector<__int16> moves;
	if (board.whiteTurn)
		moves = board.generateWhiteLegalMoves();
	else
		moves = board.generateBlackLegalMoves();
	if (depth == 1) return moves.size();
	board.totalPly++;
	board.whiteTurn = !board.whiteTurn;
	for (int i = 0; i < moves.size(); i++)
	{
		ArrayBoard boardCopy = ArrayBoard(board);
		boardCopy.makeMoveFixed(moves[i]);
		nodes += recursivePerft(boardCopy, depth - 1);
	}

	return nodes;
}

bool compareMoveValues(const __int16& x, const __int16& y)
{
	__int16 x2 = x;
	__int16 y2 = y;
	return (Move::getFrom(&x2) < Move::getFrom(&y2));
}

vector<string> Test::perftDivide(ArrayBoard board, int depth)
{
	vector<string> dividedMoves;
	vector<__int16> moves;
	if (board.whiteTurn)
		moves = board.generateWhiteLegalMoves();
	else
		moves = board.generateBlackLegalMoves();
	sort(moves.begin(), moves.end(), compareMoveValues);
	board.totalPly++;
	board.whiteTurn = !board.whiteTurn;
	for (int i = 0; i < moves.size(); i++)
	{
		ArrayBoard boardCopy = ArrayBoard(board);
		boardCopy.makeMove(moves[i]);
		unsigned long nodes = recursivePerft(boardCopy, depth - 1);
		dividedMoves.push_back(IO::convertMoveToAlg(moves[i]) + " " + to_string(nodes));
	}
	return dividedMoves;
}

char Test::pieceToChar(int piece)
{
	switch (piece)
	{
	case 1: //White pieces
		return 'P';
	case 2:
		return 'R';
	case 3:
		return 'N';
	case 4:
		return 'B';
	case 5:
		return'Q';
	case 6:
		return 'K';
	case 11: //Black pieces
		return 'p';
	case 12:
		return 'r';
	case 13:
		return 'n';
	case 14:
		return 'b';
	case 15:
		return 'q';
	case 16:
		return 'k';
	case 0: //Space
		return '.';
	}
}

string Test::pieceToString(int piece)
{
	switch (piece)
	{
	case 1: //White pieces
		return "WhitePawn";
	case 2:
		return "WhiteRook";
	case 3:
		return "WhiteKnight";
	case 4:
		return "WhiteBishop";
	case 5:
		return "WhiteQueen";
	case 6:
		return "WhiteKing";
	case 11: //Black pieces
		return "BlackPawn";
	case 12:
		return "BlackRook";
	case 13:
		return "BlackKnight";
	case 14:
		return "BlackBishop";
	case 15:
		return "BlackQueen";
	case 16:
		return "BlackKing";
	case 0: //Space
		return "Empty";
	}
}

//Static class, declaration never used

Test::Test()
{
}


Test::~Test()
{
}
