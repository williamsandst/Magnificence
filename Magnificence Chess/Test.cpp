#include "stdafx.h"
#include "Test.h"
#include "Move.h"
#include "IO.h"
#include <ctime>
#include "stdafx.h"
#include <algorithm>

//Static class used for testing purposes

string Test::displayBoard(BitBoard board)
{
	string boardString = "\n";
	int c = 63;
	for (size_t y = 0; y < 8; y++)
	{
		for (size_t x = 0; x < 8; x++)
		{
			boardString += pieceToChar(board.mailBox[c]);
			c--;
		}
		boardString += "\n";
	}
	boardString += "\n";
	return boardString;

}

u64 Test::perft(int depth, int startDepth, BitBoard *bb, bool color, u32 *start, HashEntryPerft *Hash, u32 tableSize)
{
	//if (depth == 0)
		//return 1;
	u32 *end;
	if (color)
		end = bb->WhiteLegalMoves(start);
	else
		end = bb->BlackLegalMoves(start);
	if (depth == 1)
	{
		//return 1;
		if (end - start == 1)
		{
			if (*start == 0 || *start == 1)
			{
				return 0;
			}
		}
		return end - start;
	}
	else
	{
		HashEntryPerft *thisPos = (Hash + (((bb->zoobristKey & tableSize) * 2)));
		if (thisPos->key == bb->zoobristKey && thisPos->depth == depth)
		{
			return thisPos->Result;
		}
		else if ((thisPos + 1)->key == bb->zoobristKey && (thisPos + 1)->depth == depth)
		{
			return (thisPos + 1)->Result;
		}
		u32 *nextStart = (start + 218);
		u64 res = 0;
		depth--;
		color = !color;
		while (start != end)
		{
			u32 move = *start;
			start++;
			if (move != 0 && move != 1)
			{
				//BitBoard c;
				//c.Copy(bb);
				//c.MakeMove(move);
				bb->MakeMove(move);
				res += perft(depth, startDepth, bb, color, nextStart, Hash, tableSize);
				bb->UnMakeMove(move);
			}
		}
		if (thisPos->depth < depth)
		{
			thisPos->key = bb->zoobristKey;
			thisPos->depth = depth + 1;
			thisPos->Result = res;
		}
		else
		{
			(thisPos + 1)->key = bb->zoobristKey;
			(thisPos + 1)->depth = depth + 1;
			(thisPos + 1)->Result = res;
		}
		return res;
	}
}

u64 Test::perft(int depth, BitBoard *bb, bool color, u32 *start)
{
	//if (depth == 0)
	//return 1;
	u32 *end;
	if (color)
		end = bb->WhiteLegalMoves(start);
	else
		end = bb->BlackLegalMoves(start);
	if (depth == 1)
	{
		//return 1;
		if (end - start == 1)
		{
			if (*start == 0 || *start == 1)
			{
				return 0;
			}
		}
		return end - start;
	}
	else
	{
		u32 *nextStart = (start + 218);
		u64 res = 0;
		depth--;
		color = !color;
		while (start != end)
		{
			u32 move = *start;
			start++;
			if (move != 0 && move != 1)
			{
				bb->MakeMove(move);
				res += perft(depth, bb, color, nextStart);
				bb->UnMakeMove(move);
			}
		}
		return res;
	}
}

bool compareMoveValue(const u32& x, const u32& y)
{
	u32 x2 = x, y2 = y;
	return ((x2 & (0b111111)) > (y2 & (0b111111)));
}

string Test::perftDivide(int depth, BitBoard *bb, bool color, u32 *start)
{
	string result = "";
	u32 *end;
	u64 total = 0;
	u64 res = 0;
	if (color)
	{
		end = bb->WhiteLegalMoves(start);
	}
	else
	{
		end = bb->BlackLegalMoves(start);
	}
	u32* nextStart = start + 218;
	depth--;
	color = !color;
	while (start != end)
	{
		u32 move = *start;
		start++;
		if (move != 0 && move != 1)
		{
			bb->MakeMove(move);
			res = perft(depth, bb, color, nextStart);
			total += res;
			result += IO::convertMoveToAlg(move) + "    " + to_string(res) + "\n";
			bb->UnMakeMove(move);
		}
	}
	result += "Total count: " + to_string(total) + '\n';
	return result;
}

char Test::pieceToChar(int piece)
{
	switch (piece)
	{
	case 5: //White pieces
		return 'P';
	case 3:
		return 'R';
	case 4:
		return 'N';
	case 2:
		return 'B';
	case 1:
		return 'Q';
	case 0:
		return 'K';
	case 12: //Black pieces
		return 'p';
	case 10:
		return 'r';
	case 11:
		return 'n';
	case 9:
		return 'b';
	case 8:
		return 'q';
	case 7:
		return 'k';
	case 14: //Space
		return '.';
	}
	return 0;
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
	return "Broken";
}

//Debug functions for BitBoard
//bool Test::tester(int depth, BitBoard *bb, bool color, int startDepth)
//{
//	if (hasBeenCorrupted(bb))
//	{
//		return true;
//	}
//	if (depth > 0)
//	{
//		//cnt++;
//		//cout << to_string(cnt) << endl;
//		depth--;
//		vector<u32> moves;
//		if (color)
//		{
//			moves = bb->WhiteLegalMoves();
//		}
//		else
//		{
//			moves = bb->BlackLegalMoves();
//		}
//		for each (u32 move in moves)
//		{
//			bool value = false;
//			if (move == ~((u32)0))
//			{
//				continue;
//			}
//			if (!bb->MakeMove(move))
//			{
//				seePos(bb);
//				cout << to_string(depth) << "    " << to_string(color);
//				cout << endl;
//				return true;
//			};
//			if (hasBeenCorrupted(bb))
//			{
//				value = true;
//			}
//			value = value || tester(depth, bb, !color, startDepth);
//			bb->UnMakeMove(move);
//			if (hasBeenCorrupted(bb))
//			{
//				value = true;
//			}
//			if (value)
//			{
//				cout << "\n\n";
//				cout << endl << (move & 0b111111) << "  to  " << ((move >> 6) & 63) << "   piece moved: " << to_string(bb->mailBox[(move & 0b111111)]) << "  piece taken: " << to_string(bb->mailBox[((move >> 6) & 63)]) << "   Turn: " << to_string(color) << "  EP: " << ((move >> 20) & 1) << "   ply: " << to_string(depth) << " EP state: " << to_string(bb->EP);
//				seePos(bb);
//				bb->MakeMove(move);
//				cout << endl << (move & 0b111111) << "  to  " << ((move >> 6) & 63) << "   piece moved: " << to_string(bb->mailBox[((move >> 6) & 63)]) << "  piece taken: " << to_string(bb->mailBox[((move >> 0) & 63)]) << "   Turn: " << to_string(color) << "  EP: " << ((move >> 20) & 1) << "   ply: " << to_string(depth) << " EP state: " << to_string(bb->EP);
//				seePos(bb);
//				bb->UnMakeMove(move);
//				return true;
//			}
//		}
//	}
//	return false;
//}

void Test::seePos(BitBoard *bb)
{
	for (int i = 7; i >= 0; i--)
	{
		cout << "\n";
		for (int i2 = 7; i2 > -1; i2--)
		{
			u32 index = i2 + i * 8;
			if (bb->mailBox[index] > 14)
			{
				cout << "Terrible";
				continue;
			}
			int color = 14;
			if (bb->mailBox[index] < 6)
			{
				color = 6;
			}
			else if (bb->mailBox[index] < 14)
			{
				color = 13;
			}
			bool mem = false;
			{
				for (int i = 0; i < 14; i++)
				{
					if (bb->Pieces[i] & (bb->one << index) && (bb->mailBox[index] != i) && color != i)
					{
						mem = true;
					}
				}
			}
			if (((!(bb->Pieces[bb->mailBox[index]] & bb->Pieces[color] & (bb->one << index))) && (bb->mailBox[index] != 14)) || mem)
			{
				cout << "t";
			}
			else
			{
				char out;
				switch (bb->mailBox[index])
				{
				case 14:
					out = '.';
					break;
				case 5:
					out = 'P';
					break;
				case 12:
					out = 'p';
					break;
				case 4:
					out = 'N';
					break;
				case 11:
					out = 'n';
					break;
				case 3:
					out = 'R';
					break;
				case 10:
					out = 'r';
					break;
				case 2:
					out = 'B';
					break;
				case 9:
					out = 'b';
					break;
				case 1:
					out = 'Q';
					break;
				case 8:
					out = 'q';
					break;
				case 0:
					out = 'K';
					break;
				case 7:
					out = 'k';
					break;
				default:
					out = 'w';
					break;
				}
				cout << out;
			}
		}
	}
	cout << "\n________";
}

bool Test::hasBeenCorrupted(BitBoard *bb)
{
	if (bb->pc(bb->Pieces[0]) != 1 || bb->pc(bb->Pieces[7]) != 1)
	{
		return true;
	}
	for (int i = 7; i >= 0; i--)
	{
		for (int i2 = 7; i2 > -1; i2--)
		{
			u32 index = i2 + i * 8;
			if (bb->mailBox[index] > 14)
			{
				cout << "Terrible";
				continue;
			}
			int color = 14;
			if (bb->mailBox[index] < 6)
			{
				color = 6;
			}
			else if (bb->mailBox[index] < 14)
			{
				color = 13;
			}
			bool mem = false;
			{
				for (int i = 0; i < 14; i++)
				{
					if (bb->Pieces[i] & (bb->one << index) && (bb->mailBox[index] != i) && color != i)
					{
						mem = true;
					}
				}
			}
			if (((!(bb->Pieces[bb->mailBox[index]] & bb->Pieces[color] & (bb->one << index))) && (bb->mailBox[index] != 14)) || mem)
			{
				return true;
			}
		}
	}
	return false;
}


//Static class, declaration never used

Test::Test()
{
}


Test::~Test()
{
}
