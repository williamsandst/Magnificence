#include "ArrayBoard.h"

//Data values for board array
//White:
//1 = pawn
//2 = rook
//3 = knight
//4 = bishop
//5 = queen
//6 = king
//Black
//11 = pawn
//12 = rook
//13 = knight
//14 = bishop
//15 = queen
//16 = king

void ArrayBoard::makeMove(u16 move)
{
	i8 fromPos = (move >> 10) & 63;
	//Castling and king moves
	if (board[fromPos] == 6) //Trying to move king
	{
		if (fromPos == 60) //Is the king in starting postion? White castling check
		{
			if (castlingWhiteKingSide && ((move >> 4) & 63) == 62) //King side
			{
				//Perform castling
				board[60] = 0;
				board[61] = 2;
				board[62] = 6;
				board[63] = 0;
				castlingWhiteKingSide = false;
				castlingWhiteQueenSide = false;
				return;
			}
			else if (castlingWhiteQueenSide && ((move >> 4) & 63) == 58 && board[57] == 0) //Queen side
			{
				//Perform castling
				board[60] = 0;
				board[59] = 2;
				board[58] = 6;
				board[56] = 0;
				castlingWhiteKingSide = false;
				castlingWhiteQueenSide = false;
				return;
			}
		}
		//If white king is moving, castling can no longer be performed
		castlingWhiteKingSide = false;
		castlingWhiteQueenSide = false;
		board[(move >> 4) & 63] = board[fromPos];
		board[fromPos] = 0;
		return;
	}
	else if (board[fromPos] == 16) //Check if the black king is moving
	{
		if (fromPos == 4) //Is black king in starting postion? Black castling check
		{
			if (castlingBlackKingSide && ((move >> 4) & 63) == 6) //King side
			{
				//Perform castling
				board[4] = 0;
				board[5] = 12;
				board[6] = 16;
				board[7] = 0;
				castlingBlackKingSide = false;
				castlingBlackQueenSide = false;
				return;
			}
			else if (castlingBlackQueenSide && ((move >> 4) & 63) == 2 && board[1] == 0) //Queen side
			{
				//Perform castling
				board[4] = 0;
				board[3] = 12;
				board[2] = 16;
				board[0] = 0;
				castlingBlackKingSide = false;
				castlingBlackQueenSide = false;
				return;
			}
		}
		//If black king is moving, castling can no longer be performed
		castlingBlackKingSide = false;
		castlingBlackQueenSide = false;
		board[(move >> 4) & 63] = board[fromPos];
		board[fromPos] = 0;
		return;
	}
	//Specific piece rules
	//Moving rooks stops castling
	if (board[fromPos] == 6) //White rook
	{
		if (fromPos == 63) //White kingside rook is moving, disable kingside castling
		{
			castlingWhiteKingSide = false;
			board[(move >> 4) & 63] = board[fromPos];
			board[fromPos] = 0;
			return;
		}
		if (fromPos == 56) //White queenside rook is moving, disable queenside castling
		{
			castlingWhiteQueenSide = false;
			board[(move >> 4) & 63] = board[fromPos];
			board[fromPos] = 0;
			return;
		}
	}
	else if (board[fromPos] == 16) //Black rook
	{
		if (fromPos == 7) //Black kingside rook is moving, disable kingside castling
		{
			castlingBlackKingSide = false;
			board[(move >> 4) & 63] = board[fromPos];
			board[fromPos] = 0;
			return;
		}
		if (fromPos == 0) //Black queenside rook is moving, disable queenside castling
		{
			castlingBlackQueenSide = false;
			board[(move >> 4) & 63] = board[fromPos];
			board[fromPos] = 0;
			return;
		}
	}
	
	//Pawn promotion
	if (board[fromPos] == 1 && ((move >> 4) & 63) < 8) //White pawn promoted
	{
		board[fromPos] = 0;
		board[(move >> 4) & 63] = 5; //Currently automaticly a queen
		return;
	}

	if (board[fromPos] == 11 && ((move >> 4) & 63) > 55) //Black pawn promoted
	{
		board[fromPos] = 0;
		board[(move >> 4) & 63] = 15; //Currently automaticly a queen
		return;
	}

	//En passant left!
	
	board[(move >> 4) & 63] = board[fromPos];
	board[fromPos] = 0;
	return;
}

void ArrayBoard::undoMove(u16 move)
{

}


ArrayBoard::ArrayBoard()
{
}

ArrayBoard::ArrayBoard(string fenString)
{
}


ArrayBoard::~ArrayBoard()
{
}
