// Magnificence Chess.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <thread>
#include <windows.h>
#include <sstream>
#include <chrono>
#include <atomic>
#include "Board.h"
#include "GameState.h"
#include <algorithm>
#include "ArrayBoard.h"
#include "IO.h"

using namespace std;

void DebugWrite(wchar_t* msg);
void runEngine(GameState* gameState);
void guiInterface();

void DebugWrite(wchar_t* msg) { OutputDebugStringW(msg); }
//Sample for Debug: DebugWrite(L"Hello World!")
int cnt = 0;
//Sent 

void seePos(BitBoard *bb)
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
			if (((!(bb->Pieces[bb->mailBox[index]] & bb->Pieces[color]  & (bb->one << index))) && (bb->mailBox[index] != 14)) || mem)
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

bool hasBeenCorrupted(BitBoard *bb)
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

bool tester(int depth, BitBoard *bb, bool color, int startDepth)
{
	if (hasBeenCorrupted(bb))
	{
		return true;
	}
	if (depth > 0)
	{
		cnt++;
		//cout << to_string(cnt) << endl;
		depth--;
		vector<u32> moves;
		if (color)
		{
			moves = bb->WhiteLegalMoves();
		}
		else
		{
			moves = bb->BlackLegalMoves();
		}
		for each (u32 move in moves)
		{
			bool value = false;
			if (move == ~((u32)0))
			{
				continue;
			}
			if (!bb->MakeMove(move))
			{
				seePos(bb);
				cout << to_string(depth) << "    " << to_string(color);
				cout << endl;
				return true;
			};
			if (hasBeenCorrupted(bb))
			{
				value = true;
			}
			value = value || tester(depth, bb, !color, startDepth);
			bb->UnMakeMove(move);
			if (hasBeenCorrupted(bb))
			{
				value = true;
			}
			if (value)
			{
				cout << "\n\n";
				cout << endl << (move & 0b111111) << "  to  " << ((move >> 6) & 63) << "   piece moved: " << to_string(bb->mailBox[(move & 0b111111)]) << "  piece taken: " << to_string(bb->mailBox[((move >> 6) & 63)]) << "   Turn: " << to_string(color) << "  EP: " << ((move >> 20) & 1) << "   ply: " << to_string(depth) << " EP state: " << to_string(bb->EP);
				seePos(bb);
				bb->MakeMove(move);
				cout << endl << (move & 0b111111) << "  to  " << ((move >> 6) & 63) << "   piece moved: " << to_string(bb->mailBox[((move >> 6) & 63)]) << "  piece taken: " << to_string(bb->mailBox[((move >> 0) & 63)]) << "   Turn: " << to_string(color) << "  EP: " << ((move >> 20) & 1) << "   ply: " << to_string(depth) << " EP state: " << to_string(bb->EP);
				seePos(bb);
				bb->UnMakeMove(move);
				return true;
			}
		}
	}
	return false;
}

u64 perft(int depth, BitBoard *bb, bool color)
{
	vector<u32> moves;
	if (color)
	{
		moves = bb->WhiteLegalMoves();
	}
	else
	{
		moves = bb->BlackLegalMoves();
	}
	if (depth < 1)
	{
		return 1;
		if (moves.size() == 1)
		{
			if (moves[0] == 0 || moves[0] == (~((u32)0)))
			{
				return 0;
			}
		}
		return moves.size();
	}
	else
	{
		u64 res = 0;
		for each (u32 move in moves)
		{
			if (move != 0 && move != (~((u32)0)))
			{
				bb->MakeMove(move);
				res += perft(depth - 1, bb, !color);
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

string toAlg(u32 move)
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

void Divide(u64 res, int depth, BitBoard *bb, bool color)
{
	vector<u32> moves;
	if (color)
	{
		moves = bb->WhiteLegalMoves();
	}
	else
	{
		moves = bb->BlackLegalMoves();
	}
	sort(moves.begin(), moves.end(), compareMoveValue);
	for each (u32 move in moves)
	{
		if (move != 0 && move != (~((u32)0)))
		{
			bb->MakeMove(move);
			res = perft(depth - 1, bb, !color);
			cout << toAlg(move) << "    " << to_string(res) << "\n";
			bb->UnMakeMove(move);
		}
	}
}

int main()
{
	BitBoard *bb = new BitBoard("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
	seePos(bb);
	bool color = 0;
	int perftDepth = 6;
	u64 cnt = perft(1, bb, color);
	cout << "\n" << to_string(cnt) << "\n";
	chrono::high_resolution_clock::time_point start = chrono::high_resolution_clock::now(), end;
	cnt = perft(perftDepth, bb, color);
	end = chrono::high_resolution_clock::now();
	chrono::duration<double> taken = chrono::duration_cast<chrono::duration<double>>(end - start);
	cout << "Perft " << to_string(perftDepth) <<  " took " << to_string(taken.count()) << " seconds at " << to_string(cnt / (taken.count() * 1000)) << " kpos/s";
	cout << "\n" << to_string(cnt) << "\n";
	//Divide(0, 1, bb, true);
	seePos(bb);
	string returned;
	cin >> returned;
	//DebugWrite(L"Program started");
	//guiInterface();
	//DebugWrite(L"Program terminated");
    return 0;
}

//This function recieves input from a separate GUI in the form of UCI commands.
void guiInterface()
{
	//Settings
	const bool CONSOLEDEBUG = true;
	//Create engine thread object
	GameState* gameState = new GameState();
	
	gameState->idle = true;
	gameState->run = true;	
	thread engineThread(runEngine, gameState);


	string recievedCommand;
	cout.setf(ios::unitbuf);

	int flag = 1;

	ArrayBoard board = ArrayBoard();
	board = IO::convertFENtoArrayBoard("rnbqkbnr/8/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

	while (getline(cin, recievedCommand))
	{
		if (CONSOLEDEBUG)
		{
			vector<string> splitCommand = split(recievedCommand, ' ');
			if (splitCommand[0] == "test" || splitCommand[0] == "t")
			{
				if (splitCommand[1] == "display" || splitCommand[1] == "disp")
				{
					cout << IO::displayBoard(board);
				}
				if (splitCommand[1] == "move")
				{
					board.makeMove(IO::convertAlgToMove(splitCommand[2]));
				}
			}
		}
		if (recievedCommand == "uci")
		{
			cout << "id name Magnificence" << endl;
			cout << "id author HarWil" << endl;
			cout << "uciok" << endl;
		}
		else if (recievedCommand == "quit")
		{
			cout << "Exiting interface..." << endl;
			break;
		}
		else if (recievedCommand == "ucinewgame")
		{

		}

		if (recievedCommand.substr(0, 23) == "position startpos moves ") 
		{

		}
		else if (recievedCommand == "stop") 
		{

		}
		else if (recievedCommand.substr(0, 3) == "go ") {
			// Received command in this format: "go wtime 300000 btime 300000 winc 0 binc 0"
			cout << "bestmove " << char(105 - flag) << "7" << char(105 - flag) << "5" << endl;
			//Output format: "bestmove h7h5"
			flag++; //increase flag to move other pawn on next turn
			//gameState->idle = !(gameState->idle);
		}
	}
	gameState->run = false;
	engineThread.detach();
	return;
}

//The chess engine will run here. Everything that needs to be passed to the GUI is stored in GameState
//More variables can be added to gamestate if necessary.
void runEngine(GameState* gameState)
{
	while (gameState->run)
	{
		this_thread::sleep_for(chrono::milliseconds(1));
		while (!gameState->idle)
		{
			cout << "Test";
		}
	}
}


