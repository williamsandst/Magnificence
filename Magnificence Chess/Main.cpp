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
u64 cnt = 0;
//Sent 

struct Move
{
	u32 Moves[218];
};

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
	cout <<"\n" << to_string(bb->EP) << "   "  << to_string(bb->rockad) << "   " << to_string(bb->silent)<<"\n________";
}

bool hasBeenCorrupted(BitBoard *bb)
{
	if (bb->pc(bb->Pieces[0]) != 1)
	{
		seePos(bb);
		return true;
	}
	if (bb->pc(bb->Pieces[7]) != 1)
	{
		seePos(bb);
		return true;
	}
	if (((bb->pc(bb->Pieces[1])) > 1) || (bb->pc(bb->Pieces[8]) > 1))
	{
		cout << to_string(bb->pc(bb->Pieces[1])) << "   " << to_string(bb->pc(bb->Pieces[8])) << "   " << to_string((bb->pc(bb->Pieces[1] > 1)) || (bb->pc(bb->Pieces[8] > 1))) << "\n";
		seePos(bb);
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

u64 perft(int depth, BitBoard *bb, bool color, Move *object)
{
	u32 *Start = object->Moves, *End;
	if (color)
	{
		End = bb->WhiteLegalMoves(Start);
	}
	else
	{
		End = bb->BlackLegalMoves(Start);
	}
	if (depth < 2)
	{
		if (End - Start == 1)
		{
			if (*Start == 0 || *Start == 1)
			{
				return 0;
			}
		}
		return (u64)(End - Start);
	}
	{
		u64 res = 0;
		depth--;
		object++;
		color = !color;
		while (Start != End)
		{
			u32 move = *Start;
			Start++;
			if (move != 0 && move != 1)
			{
				bb->MakeMove(move);
				res += perft(depth , bb, color, object);
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

u64 Divide(u64 res, int depth, BitBoard *bb, bool color, Move *object)
{
	u32 *start = object->Moves, *end;
	u64 ret = 0;
	if (color)
	{
		end = bb->WhiteLegalMoves(start);
	}
	else
	{
		end = bb->BlackLegalMoves(start);
	}
	depth--;
	color = !color;
	object++;
	while (start != end)
	{
		u32 move = *start;
		if (move != 0 && move != 1)
		{
			bb->MakeMove(move);
			res = perft(depth, bb, color, object);
			ret += res;
			cout << to_string(move) << "   " << toAlg(move) << "    " << to_string(res) << "\n";
			bb->UnMakeMove(move);
		}
		start++;
	}
	return ret;
}

int main()
{
	BitBoard *bb = new BitBoard("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq -");
	//seePos(bb);
	bool color = 1;
	int perftDepth = 6;
	Move *MoveObjectArray = new Move[perftDepth + 1];
	chrono::high_resolution_clock::time_point start = chrono::high_resolution_clock::now(), end;
	cnt = perft(perftDepth, bb, color, MoveObjectArray);
	//cnt = Divide(0,perftDepth, bb, color, MoveObjectArray);//Divide(0, perftDepth, bb, color);
	end = chrono::high_resolution_clock::now();
	chrono::duration<double> taken = chrono::duration_cast<chrono::duration<double>>(end - start);
	cout << endl << "Perft " << to_string(perftDepth) <<  " took " << to_string(taken.count()) << " seconds at " << to_string(cnt / (taken.count() * 1000)) << " kpos/s";
	cout << "\n" << to_string(cnt) << "\n";
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


