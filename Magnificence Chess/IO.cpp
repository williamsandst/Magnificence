#include "stdafx.h"
#include "IO.h"

//Static class, used for input/output to interface, converts between different internal
//and external notations


//Converting between inherent move object and long algebraic form used by interpeter
string IO::convertMoveToAlg(__int16 move)
{
	string alg;
	__int16 output1 = (move >> 4) & 63;
	__int16 output2 = (move >> 10) & 63;
	alg += output2 % 8 + 'a';
	alg += 8 - output2 / 8 + '0';
	alg += output1 % 8 + 'a';
	alg += 8 - output1 / 8 + '0';
	return alg;
}

__int16 IO::convertAlgToMove(string alg)
{
	int a = (((8 - (alg[1] - '0')) * 8 + (alg[0] - 'a')) << 10);
	int b = (((8 - (alg[3] - '0')) * 8 + (alg[2] - 'a')) << 4);
	return (a | b);
}

//Convert boards to FEN
string IO::convertBoardToFEN(ArrayBoard board)
{
	string fenString;
	int emptyCounter = 0;
	//Positions
	for (int y = 0; y < 8; y++)
	{
		for (int x = 0; x < 8; x++)
		{
			if (emptyCounter != 0 && board.board[y * 8 + x] != 0)
			{
				fenString += to_string(emptyCounter);
				emptyCounter = 0;
			}
			switch (board.board[y * 8 + x])
			{
			case 1: //White pieces
				fenString += 'P';
				break;
			case 2:
				fenString += 'R';
				break;
			case 3:
				fenString += 'N';
				break;
			case 4:
				fenString += 'B';
				break;
			case 5:
				fenString += 'Q';
				break;
			case 6:
				fenString += 'K';
				break;
			case 11: //Black pieces
				fenString += 'p';
				break;
			case 12:
				fenString += 'r';
				break;
			case 13:
				fenString += 'n';
				break;
			case 14:
				fenString += 'b';
				break;
			case 15:
				fenString += 'q';
				break;
			case 16:
				fenString += 'k';
				break;
			case 0: //Space
				emptyCounter++;
				break;
			}
		}
		if (emptyCounter != 0)
		{
			fenString += to_string(emptyCounter);
			emptyCounter = 0;
		}
		if (y != 7)
			fenString += '/';
	}
	fenString += " ";
	//Color turn
	fenString += ((board.totalPly % 2) == 1) ? 'w' : 'b';
	fenString += " ";
	//Castling
	if (!(board.castlingWhiteKingSide || board.castlingWhiteQueenSide
		|| board.castlingBlackKingSide || board.castlingBlackQueenSide))
	{
		fenString += "-";
	}
	else
	{
		fenString += (board.castlingWhiteKingSide ? "K" : "");
		fenString += (board.castlingWhiteQueenSide ? "Q" : "");
		fenString += (board.castlingBlackKingSide ? "k" : "");
		fenString += (board.castlingBlackQueenSide ? "q" : "");
	}
	fenString += " ";
	//En passant
	if (board.enPassantSquare < 0) //No en passant square
		fenString += "-";
	else //Add en passant square in algebraic format
	{
		fenString += board.enPassantSquare % 8 + 'a';
		fenString += 8 - board.enPassantSquare / 8 + '0';
	}
	//Turns
	fenString += " " + to_string(board.drawCounter);
	fenString += " " + to_string((board.totalPly + 1) / 2);

	return fenString;
}

string IO::convertBoardToFEN(BitBoard board)
{
	return string();
}

//Converting FEN to Boards
BitBoard IO::convertFENtoBitBoard(string fenString)
{
	return BitBoard();
}

ArrayBoard IO::convertFENtoArrayBoard(string fenString)
{
	ArrayBoard board = ArrayBoard();
	//Set all values to 0
	for (size_t i = 0; i < 64; i++)
	{
		board.board[i] = 0;
	}
	vector<string> fenParts = split(fenString, ' ');
	int x = 0;
	int y = 0;
	for (size_t i = 0; i < fenParts[0].length(); i++) //Positions
	{
		switch (fenParts[0][i])
		{
		case 'P': //White pieces
			board.board[y * 8 + x] = 1;
			break;
		case 'R':
			board.board[y * 8 + x] = 2;
			break;
		case 'N':
			board.board[y * 8 + x] = 3;
			break;
		case 'B':
			board.board[y * 8 + x] = 4;
			break;
		case 'Q':
			board.board[y * 8 + x] = 5;
			break;
		case 'K':
			board.board[y * 8 + x] = 6;
			break;
		case 'p': //Black pieces
			board.board[y * 8 + x] = 11;
			break;
		case 'r':
			board.board[y * 8 + x] = 12;
			break;
		case 'n':
			board.board[y * 8 + x] = 13;
			break;
		case 'b':
			board.board[y * 8 + x] = 14;
			break;
		case 'q':
			board.board[y * 8 + x] = 15;
			break;
		case 'k':
			board.board[y * 8 + x] = 16;
			break;
		case '/':
			y++;
			x = -1;
			break;
		default: //If none of the above, it must be a number, representing a space.
			//Convert to int and add it to x
			//Remove 1 due to adding of 1 to x further down
			x += (fenParts[0][i] - '0') - 1;
			break;
		}
		x++;
		if (y == 7 && x == 8) //Done with positions
		{
			break;
		}
	}
	//Next step: Castling
	(fenParts[2].find('K') != string::npos) ? board.castlingWhiteKingSide = true : board.castlingWhiteKingSide = false;
	(fenParts[2].find('Q') != string::npos) ? board.castlingWhiteQueenSide = true : board.castlingWhiteQueenSide = false;
	(fenParts[2].find('k') != string::npos) ? board.castlingBlackKingSide = true : board.castlingBlackKingSide = false;
	(fenParts[2].find('q') != string::npos) ? board.castlingBlackQueenSide = true : board.castlingBlackQueenSide = false;
	//En passant
	if (fenParts[3] != "-")
		board.enPassantSquare = (8 - (fenParts[3][1] - '0')) * 8 + (fenParts[3][0] - 'a');
	else
		board.enPassantSquare = -1;
	//Last step: Information about Turn and Ply
	board.drawCounter = stoi(fenParts[4]);
	//Totalply = turn * 2 - (if white)
	//Example:
	//1: White (ply 1) 1 * 2 - 1
	//1: black (ply 2) 1 * 2 - 0
	//2: White (ply 3) 2 * 2 - 1
	//2: Black (ply 4) 2 * 2 - 0
	board.totalPly = stoi(fenParts[5]) * 2 - (fenParts[1] == "w");

	//Piece values
	/*White:
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
	//16 = king*/
	return board;
}

//Returns a string representing the board in a cool format, used for debugging
string IO::displayBoard(ArrayBoard board)
{
	string boardString = "\n";
	for (size_t y = 0; y < 8; y++)
	{
		for (size_t x = 0; x < 8; x++)
		{
			switch (board.board[y * 8 + x])
			{
			case 1: //White pieces
				boardString += 'P';
				break;
			case 2:
				boardString += 'R';
				break;
			case 3:
				boardString += 'N';
				break;
			case 4:
				boardString += 'B';
				break;
			case 5:
				boardString += 'Q';
				break;
			case 6:
				boardString += 'K';
				break;
			case 11: //Black pieces
				boardString += 'p';
				break;
			case 12:
				boardString += 'r';
				break;
			case 13:
				boardString += 'n';
				break;
			case 14:
				boardString += 'b';
				break;
			case 15:
				boardString += 'q';
				break;
			case 16:
				boardString += 'k';
				break;
			case 0: //Space
				boardString += ".";
				break;
			}
		}
		boardString += "\n";
	}
	boardString += "\n";
	return boardString;

}

//Static class, declaration is NEVER used
IO::IO()
{
}


IO::~IO()
{
}

vector<string> split(string input, char delimiter)
{
	vector<string> output;
	int foundSymbolAt = 0;
	int foundSymbolAtOld = -1;
	int i = 0;
	while (foundSymbolAt != -1)
	{
		foundSymbolAt = input.find(delimiter, foundSymbolAtOld + 1);
		output.push_back(input.substr(foundSymbolAtOld + 1, foundSymbolAt - foundSymbolAtOld - 1));
		foundSymbolAtOld = foundSymbolAt;
		i++;
	}
	return output;
}