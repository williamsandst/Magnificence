// Magnificence Chess.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <thread>
#include <windows.h>
#include <sstream>
#include <chrono>
#include "ABAI.h"
#include <atomic>
#include <ctime>
#include "Board.h"
#include "GameState.h";
#include "BitBoard.h";
#include "IO.h";
#include "Move.h"
#include "Test.h";
#include "Engine.h";

string commandList =
"\nCommand list for Magnificence Chess Engine Development Build \n"
"disp			Display the board\n"
"move	<MOVE>		Perform a move\n"
"perft	<DEPTH>		Calculate the perft score for current position\n"
"divide	<DEPTH>		Divide the perft on first depth for debugging\n"
"moves	<COLOR>		Display legal moves at current position\n"
"setboard <FEN>		Set the board to FEN position\n"
"fen			Output a fen string for current position\n"
"uci			Enables uci-mode and gives control to a GUI\n\n";

using namespace std;

void DebugWrite(wchar_t* msg);
void runEngine(GameState* gameState);
void guiInterface();

void DebugWrite(wchar_t* msg) { OutputDebugStringW(msg); }
//Sample for Debug: DebugWrite(L"Hello World!")

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
	bool CONSOLEDEBUG = true;
	//Create engine thread object
	GameState* gameState = new GameState();

	gameState->idle = true;
	gameState->run = true;
	thread engineThread(runEngine, gameState);


	string recievedCommand;
	cout.setf(ios::unitbuf);

	int flag = 1;
	bool unknownCommand = false;

	clock_t timer;
	double duration;
	cout << "------------------------------" << endl;
	cout << "Magnificence Development Build" << endl;
	cout << "------------------------------" << endl << endl;


	cout << "Generating magic tables..." << endl;
	BitBoard board = BitBoard("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
	gameState->board = &board;
	cout << "Magic tables complete." << endl << endl;
	cout << "For help, type 'help'." << endl << endl;

	cout << "mgnf: ";
	bool color = true;
	while (getline(cin, recievedCommand))
	{
		vector<string> splitCommand = split(recievedCommand, ' ');
		if (CONSOLEDEBUG) //Debug commands, not used for interface!
		{
			if (splitCommand[0] == "help" || splitCommand[0] == "commands")
				cout << commandList;
			else if (splitCommand[0] == "perft" && splitCommand.size() == 2)
			{
				//tablesize should be power of 2 - 1;
				u32 *start = new u32[218 * (stoi(splitCommand[1]) + 1)];
				u32 tableSize = 16777215;//8388607;
				HashEntryPerft *Hash = new HashEntryPerft[2 * tableSize + 2];
				timer = clock();
				u64 perftNumber = Test::perft(stoi(splitCommand[1]), stoi(splitCommand[1]), &board, color, start, Hash, tableSize);
				duration = (clock() - timer) / (double)CLOCKS_PER_SEC;
				cout << perftNumber << " [" << to_string(duration) << " s] " << "[" << to_string((perftNumber / 1000000.0F) / duration) << " MN/S]" << endl;
				delete[] start;
				delete[] Hash;
			}
			else if (splitCommand[0] == "fen")
				cout << IO::convertBoardToFEN(board, color) << endl;
			else if (splitCommand[0] == "display" || splitCommand[0] == "disp")
				cout << Test::displayBoard(board);
			else if (splitCommand[0] == "divide" && splitCommand.size() == 2)
			{
				u32 *start = new u32[218 * (stoi(splitCommand[1]) + 1)];
				timer = clock();
				string result = Test::perftDivide(stoi(splitCommand[1]), &board, color, start);
				duration = (clock() - timer) / (double)CLOCKS_PER_SEC;
				cout << result << endl;
				delete[] start;
			}
			else if (splitCommand[0] == "move" && splitCommand.size() == 2)
			{
				color = !color;
				board.MakeMove(IO::convertAlgToMove(splitCommand[1]));
			}
			else if ((splitCommand[0] == "moves" || splitCommand[0] == "lmov") && splitCommand.size() == 2)
			{
				if (splitCommand[1] == "white" || splitCommand[1] == "w")
				{
					u32 *start = new u32[218];
					u32 *end = board.WhiteLegalMoves(start);
					cout << "Move count: " << to_string((u32)(end - start)) << endl;
					for (size_t i = 0; i < (u32)(end - start); i++)
					{
						cout << IO::convertMoveToAlg(start[i]) << " [" <<
							Test::pieceToString(board.mailBox[Move::getFrom(start[i])]) << " to " <<
							Test::pieceToString(board.mailBox[Move::getTo(start[i])]) << "]" << endl;
					}
					delete []start;
				}
				else if (splitCommand[1] == "black" || splitCommand[1] == "b")
				{
					u32 *moves = new u32[218];
					u32 *end = board.BlackLegalMoves(moves);
					cout << "Move count: " << to_string((u32)(end - moves)) << endl;
					for (size_t i = 0; i < (u32)(end - moves); i++)
					{
						cout << IO::convertMoveToAlg(moves[i]) << " [" <<
							Test::pieceToString(board.mailBox[Move::getFrom(moves[i])]) << " to " <<
							Test::pieceToString(board.mailBox[Move::getTo(moves[i])]) << "]" << endl;
					}
					delete[] moves;
				}
			}
			else if (splitCommand[0] == "test" || splitCommand[0] == "t" && splitCommand.size() > 2)
			{
				if (splitCommand[1] == "movetime" && splitCommand.size() == 3)
				{
					u32* start = new u32[218];
					timer = clock();
					int end = stoi(splitCommand[2]);
					for (size_t i = 0; i < end; i++)
					{
						board.WhiteLegalMoves(start);
					}
					delete [] start;
					duration = (clock() - timer) / (double)CLOCKS_PER_SEC;
					cout << splitCommand[2] << " moves generated in " << duration << " s" << endl;
				}
			}
			else
			{
				unknownCommand = true;
			}
		}
		if (recievedCommand == "uci")
		{
			CONSOLEDEBUG = false;
			unknownCommand = false;
			cout << "id name Magnificence" << endl;
			cout << "id author William" << endl;
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
		else if (splitCommand[0] == "position" && splitCommand[1] == "startpos")
		{
			board.SetState("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
			color = true;
			if (splitCommand.size() > 2 && splitCommand[2] == "moves")
			{
				for (size_t i = 3; i < splitCommand.size(); i++)
				{
					board.MakeMove(IO::convertAlgToMove(splitCommand[i]));
					color = !color;
				}
			}
		}
		else if (splitCommand[0] == "position")
		{
			string fen = splitCommand[1] + " " + splitCommand[2] + " " + splitCommand[3]
				+ " " + splitCommand[4] + " " + splitCommand[5] + " " + splitCommand[6];
			board.SetState(fen);
			if (splitCommand[7] == "moves")
			{
				for (size_t i = 8; i < splitCommand.size(); i++)
				{
					color = !color;
					board.MakeMove(IO::convertAlgToMove(splitCommand[i]));
				}
			}
		}
		else if (splitCommand[0] == "setboard")
		{
			board.SetState(recievedCommand.substr(9, recievedCommand.size()));
		}
		else if (recievedCommand == "stop")
		{

		}
		else if (splitCommand[0] == "go") {
			// Received command in this format: "go wtime 300000 btime 300000 winc 0 binc 0"
			//Output format: "bestmove h7h5"
			gameState->color = color;
			if (splitCommand.size() > 1 && (isdigit(splitCommand[1][0]) != 0))
				gameState->maxDepth = stoi(splitCommand[1]);
			BitBoard * boardPtr = &board;
			gameState->board = boardPtr;
			gameState->idle = !(gameState->idle);
		}
		else if (recievedCommand == "isready")
		{
			cout << "readyok" << endl;
		}
		else if (recievedCommand == "stop")
		{
			//Stop the engine
		}
		else if (unknownCommand)
			cout << "Unknown command. Type 'help' for a list of commands." << endl;
		if (CONSOLEDEBUG == true)
			cout << "mgnf: ";
		unknownCommand = false;

	}
	gameState->run = false;
	engineThread.detach();
	return;
}

//The chess engine will run here. Everything that needs to be passed to the GUI is stored in GameState
//More variables can be added to gamestate if necessary.
void runEngine(GameState* gameState)
{
	//Engine engine = Engine();
	ABAI *AI = new ABAI();
	while (gameState->run)
	{
		this_thread::sleep_for(chrono::milliseconds(2));
		while (!gameState->idle)
		{
			BitBoard localBB;
			localBB.Copy(gameState->board);
			//gameState->principalVariation = engine.startSearch(gameState->board, true , 0 , gameState->maxDepth);
			gameState->principalVariation = AI->bestMove(&localBB, gameState->color, CLOCKS_PER_SEC * 10, gameState->maxDepth);
			//gameState->principalVariation = engine.startSearch(gameState->board, 0, gameState->maxDepth);
			cout << "bestmove " << IO::convertMoveToAlg(gameState->principalVariation[0]) << endl;
			cout << "mgnf: ";
			gameState->idle = true;
		}
	}
}

