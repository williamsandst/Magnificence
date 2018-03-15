#include "stdafx.h"
#include "IO.h"

//Static class, used for input/output to interface, converts between different internal
//and external notations


//Converting between inherent move object and long algebraic form used by interpeter
string IO::convertMoveToAlg(u32 move)
{
	string alg;
	__int16 output2 = 63 - (move & 63);
	__int16 output1 = 63 - ((move >> 6) & 63);
	alg += output2 % 8 + 'a';
	alg += 8 - output2 / 8 + '0';
	alg += output1 % 8 + 'a';
	alg += 8 - output1 / 8 + '0';
	return alg;
}

u32 IO::convertAlgToMove(string alg)
{
	int a = (alg[1] - '0' - 1) * 8 + (7-(alg[0] - 'a'));
	int b = ((alg[3] - '0' - 1) * 8 + (7-(alg[2] - 'a'))) << 6;
	return (a | b);
}

//Convert boards to FEN

string IO::convertBoardToFEN(BitBoard board, bool color)
{
	string fenString;
	int emptyCounter = 0;
	//Positions
	for (int y = 7; y > -1; y--)
	{
		for (int x = 0; x < 8; x++)
		{
			if (emptyCounter != 0 && board.mailBox[y * 8 + x] != 14)
			{
				fenString += to_string(emptyCounter);
				emptyCounter = 0;
			}
			switch (board.mailBox[y * 8 + x])
			{
			case 5: //White pieces
				fenString += 'P';
				break;
			case 3:
				fenString += 'R';
				break;
			case 4:
				fenString += 'N';
				break;
			case 2:
				fenString += 'B';
				break;
			case 1:
				fenString += 'Q';
				break;
			case 0:
				fenString += 'K';
				break;
			case 12: //Black pieces
				fenString += 'p';
				break;
			case 10:
				fenString += 'r';
				break;
			case 11:
				fenString += 'n';
				break;
			case 9:
				fenString += 'b';
				break;
			case 8:
				fenString += 'q';
				break;
			case 7:
				fenString += 'k';
				break;
			case 14: //Space
				emptyCounter++;
				break;
			}
		}
		if (emptyCounter != 0)
		{
			fenString += to_string(emptyCounter);
			emptyCounter = 0;
		}
		if (y != 1)
			fenString += '/';
	}
	fenString += " ";
	//Color turn
	fenString += color ? 'w' : 'b';
	fenString += " ";
	//Castling
	/*if (!(board.castlingWhiteKingSide || board.castlingWhiteQueenSide
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
	*/
	return fenString;
}


//Converting FEN to Boards

/*int * IO::convertFENtoBoard(string fenString)
{
	u32 board[64];
	//Set all values to 0
	for (size_t i = 0; i < 64; i++)
	{
		board[i] = 0;
	}
	vector<string> fenParts = split(fenString, ' ');
	int x = 0;
	int y = 0;
	for (size_t i = 0; i < fenParts[0].length(); i++) //Positions
	{
		switch (fenParts[0][i])
		{
		case 'P': //White pieces
			board[y * 8 + x] = 1;
			break;
		case 'R':
			board[y * 8 + x] = 2;
			break;
		case 'N':
			board[y * 8 + x] = 3;
			break;
		case 'B':
			board[y * 8 + x] = 4;
			break;
		case 'Q':
			board[y * 8 + x] = 5;
			break;
		case 'K':
			board[y * 8 + x] = 6;
			break;
		case 'p': //Black pieces
			board[y * 8 + x] = 11;
			break;
		case 'r':
			board[y * 8 + x] = 12;
			break;
		case 'n':
			board[y * 8 + x] = 13;
			break;
		case 'b':
			board[y * 8 + x] = 14;
			break;
		case 'q':
			board[y * 8 + x] = 15;
			break;
		case 'k':
			board[y * 8 + x] = 16;
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
		board.enPassantSquare = 63 - (8 - (fenParts[3][1] - '0')) * 8 + (fenParts[3][0] - 'a');
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
	//White: king 0, queen 1, bishop 2, rook  3, knight  4, pawn 5
	//Black: king 7, queen 8, bishop 9, rook 10, knight 11, pawn 12
}*/
//Returns a string representing the board in a cool format, used for debugging
string IO::displayBoard(BitBoard board)
{
	string boardString = "\n";
	for (size_t y = 0; y < 8; y++)
	{
		for (size_t x = 0; x < 8; x++)
		{
			switch (board.mailBox[y * 8 + x])
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
	size_t foundSymbolAt = 0;
	size_t foundSymbolAtOld = -1;
	int i = 0;
	while (foundSymbolAt != -1)
	{
		foundSymbolAt = input.find(delimiter, (int)(foundSymbolAtOld + 1));
		output.push_back(input.substr(foundSymbolAtOld + 1, foundSymbolAt - foundSymbolAtOld - 1));
		foundSymbolAtOld = foundSymbolAt;
		i++;
	}
	return output;
}