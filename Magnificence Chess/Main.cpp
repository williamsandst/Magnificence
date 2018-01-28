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
#include "GameState.h";
#include "ArrayBoard.h";
#include "IO.h";

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
			if ((!(bb->Pieces[bb->mailBox[index]] & (bb->one << index))) && bb->mailBox[index] != 14)
			{
				cout << "t";
			}
			else
			{
				char out;
				switch (bb->mailBox[index])
				{
				case 14:
					out = ' ';
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
	cout << "\n________" << endl;
}

void tester(int depth, BitBoard *bb, bool color, int startDepth)
{
	if (depth > 0)
	{
		if (depth == startDepth - 1)
		{
			cnt++;
		}
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
			bb->MakeMove(move);
			tester(depth, bb, !color, startDepth);
			bb->UnMakeMove(move);
		}
	}
}

int main()
{
	BitBoard *bb = new BitBoard("4k3/pppppppp/8/8/8/8/PPPPPPPP/4K3 w - -");
	seePos(bb);
	cout << "\nWhite legal Moves " << bb->WhiteLegalMoves().size() << "\nBlack legal Moves " << bb->BlackLegalMoves().size() << endl;
	tester(7, bb, true, 7);
	cout << cnt;
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
	board = IO::convertFENtoArrayBoard("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

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


