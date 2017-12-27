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

//Sent 

int main()
{
	DebugWrite(L"Program started");
	guiInterface();
	DebugWrite(L"Program terminated");
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


