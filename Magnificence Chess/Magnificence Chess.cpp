// Magnificence Chess.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <thread>
#include "Board.h"
#include <windows.h>
#include <sstream>
#include <chrono>
#include <atomic>

using namespace std;

class GameState;
void DebugWrite(wchar_t* msg);
void runEngine(GameState* gameState);
void guiInterface();

void DebugWrite(wchar_t* msg) { OutputDebugStringW(msg); }
//Sample for Debug: DebugWrite(L"Hello World!")

//Sent 
class GameState
{
public:
	atomic<bool> run;
	GameState()
	{

	}
	~GameState()
	{

	}
	//Principal variation
	//Depth
	//Score (centipawns)
};

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
	//Create engine thread object
	GameState* gameState = new GameState();
	
	string recievedCommand;
	cout.setf(ios::unitbuf);

	int flag = 1;

	while (getline(cin, recievedCommand))
	{
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
		}
	}

	/*thread engineThread(runEngine, gameState);
	engineThread.detach();
	gameState->run = true;
	return;*/
}

//The chess engine will run here. Everything that needs to be passed to the GUI is stored in GameState
//More variables can be added to gamestate if necessary.
void runEngine(GameState* gameState)
{
	while (gameState->run)
	{
		cout << "Hello";
	}
}


