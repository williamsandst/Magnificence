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
#include "Move.h"
#include "Test.h";
#include <ctime>
#include "Engine.h";

using namespace std;

//Usage of debug commands:
//'t [command] [arguments]' or 'test [command] [arguments]'
//Command list:
//'t move [move]' (Performs move on board. move is represented in long algebraic form (ex d2d4))
//'t lmov [color]' (Gives legal moves for that color)
//'t disp (Displays the board in console)

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
	bool CONSOLEDEBUG = true;
	//Create engine thread object
	GameState* gameState = new GameState();
	
	gameState->idle = true;
	gameState->run = true;	
	thread engineThread(runEngine, gameState);


	string recievedCommand;
	cout.setf(ios::unitbuf);

	int flag = 1;

	clock_t timer;
	double duration;
	
	ArrayBoard board = ArrayBoard();
	TranspositionTable tTable = TranspositionTable();
	//Standard board;
	board = IO::convertFENtoArrayBoard("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
	//board = IO::convertFENtoArrayBoard("r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10");
	while (getline(cin, recievedCommand))
	{
		vector<string> splitCommand = split(recievedCommand, ' ');
		if (CONSOLEDEBUG) //Debug commands, not used for interface!
		{
			if (splitCommand[0] == "perft" && splitCommand.size() == 2)
			{
				timer = clock();
				unsigned long perftNumber = Test::recursivePerft(board, stoi(splitCommand[1]));
				duration = (clock() - timer) / (double)CLOCKS_PER_SEC;
				cout << perftNumber << " [" << to_string(duration) << " s] " << "[" << to_string((perftNumber / 1000000) / duration) << " MN/S]" << endl;
			}
			else if (splitCommand[0] == "fen")
					cout << IO::convertBoardToFEN(board) << endl;
			else if (splitCommand[0] == "display" || splitCommand[0] == "disp")
				cout << Test::displayBoard(board);
			else if (splitCommand[0] == "divide")
			{
				vector<string> dividedMoves = Test::perftDivide(board, stoi(splitCommand[1]));
				long totalCount = 0;
				for (size_t i = 0; i < dividedMoves.size(); i++)
				{
					cout << dividedMoves[i] << endl;
					totalCount += stoi(split(dividedMoves[i], ' ')[1]);
				}
				cout << "Total count: " << totalCount << endl;

			}
			else if (splitCommand[0] == "move" && splitCommand.size() == 2)
			{
				board.whiteTurn = !board.whiteTurn;
				board.makeMoveFixed(IO::convertAlgToMove(splitCommand[1]));
				cout << "Zobrist key: " << board.zobristKey << endl;;
			}
			else if (splitCommand[0] == "moves" || splitCommand[0] == "lmov" && splitCommand.size() == 2)
			{
				if (splitCommand[1] == "white" || splitCommand[1] == "w")
				{
					vector<__int16> moves = board.generateWhiteLegalMoves();
					cout << "Move count: " << moves.size() << endl;
					for (size_t i = 0; i < moves.size(); i++)
					{
						cout << IO::convertMoveToAlg(moves[i]) << " [" <<
							Test::pieceToString(board.board[Move::getFrom(&moves[i])]) << " to " <<
							Test::pieceToString(board.board[Move::getTo(&moves[i])]) << "]" << endl;
					}
				}
				else if (splitCommand[1] == "black" || splitCommand[1] == "b")
				{
					vector<__int16> moves = board.generateBlackLegalMoves();
					cout << "Move count: " << moves.size() << endl;
					for (size_t i = 0; i < moves.size(); i++)
					{
						cout << IO::convertMoveToAlg(moves[i]) << " [" <<
							Test::pieceToString(board.board[Move::getFrom(&moves[i])]) << " to " <<
							Test::pieceToString(board.board[Move::getTo(&moves[i])]) << "]" << endl;
					}
				}
			}
			else if (splitCommand[0] == "test" || splitCommand[0] == "t")
			{
				if (splitCommand[1] == "movetime")
				{
					timer = clock();
					int end = stoi(splitCommand[2]);
					for (size_t i = 0; i < end; i++)
					{
						board.generateWhiteLegalMoves();
					}
					duration = (clock() - timer) / (double)CLOCKS_PER_SEC;
					cout << splitCommand[2] << " moves generated in " << duration << endl;
					//board.makeMove(IO::convertAlgToMove(splitCommand[2]));
				}
				if (splitCommand[1] == "whiteattack")
				{
					//This code crashes linker
					short square = IO::convertAlgToMove(splitCommand[2]);
					bool squareBool = board.squareAttackedByWhite(Move::getFrom(&square));
					if (squareBool)
						cout << "True" << endl;
					else
						cout << "False" << endl;
				}
				if (splitCommand[1] == "blackattack")
				{
					short square = IO::convertAlgToMove(splitCommand[2]);
					bool squareBool = board.squareAttackedByBlack(Move::getFrom(&square));
					if (squareBool)
						cout << "True" << endl;
					else
						cout << "False" << endl;
				}
				if (splitCommand[1] == "whiteblock")
				{
					short square = IO::convertAlgToMove(splitCommand[2]);
					vector<__int16> moves;
					board.whiteBlockingMoves(moves, Move::getFrom(&square), vector<short>());
					cout << "Move count: " << moves.size() << endl;
					for (size_t i = 0; i < moves.size(); i++)
					{
						cout << IO::convertMoveToAlg(moves[i]) << " [" <<
							Test::pieceToString(board.board[Move::getFrom(&moves[i])]) << " to " <<
							Test::pieceToString(board.board[Move::getTo(&moves[i])]) << "]" << endl;
					}
				}
				if (splitCommand[1] == "blackblock")
				{
					short square = IO::convertAlgToMove(splitCommand[2]);
					vector<__int16> moves;
					board.blackBlockingMoves(moves, Move::getFrom(&square), vector<short>());
					cout << "Move count: " << moves.size() << endl;
					for (size_t i = 0; i < moves.size(); i++)
					{
						cout << IO::convertMoveToAlg(moves[i]) << " [" <<
							Test::pieceToString(board.board[Move::getFrom(&moves[i])]) << " to " <<
							Test::pieceToString(board.board[Move::getTo(&moves[i])]) << "]" << endl;
					}
				}
				if (splitCommand[1] == "zobrist")
				{
					board.zobristKey = tTable.getZobristKey(&board);
					cout << "Zobrist key: " << to_string(board.zobristKey) << endl;
				}
			}
		}
		if (recievedCommand == "uci")
		{
			CONSOLEDEBUG = false;
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
			board = IO::convertFENtoArrayBoard("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
			if (splitCommand.size() > 2 && splitCommand[2] == "moves")
			{
				for (size_t i = 3; i < splitCommand.size(); i++)
				{
					board.makeMove(IO::convertAlgToMove(splitCommand[i]));
				}
			}
		}
		else if (splitCommand[0] == "position")
		{
			string fen = splitCommand[1] + " " + splitCommand[2] + " " + splitCommand[3]
				+ " " + splitCommand[4] + " " + splitCommand[5] + " " + splitCommand[6];
			board = IO::convertFENtoArrayBoard(fen);
			if (splitCommand[7] == "moves")
			{
				for (size_t i = 8; i < splitCommand.size(); i++)
				{
					board.totalPly++;
					board.whiteTurn = !board.whiteTurn;
					board.makeMove(IO::convertAlgToMove(splitCommand[i]));
				}
			}
		}
		else if (splitCommand[0] == "setboard")
		{
			board = IO::convertFENtoArrayBoard(recievedCommand.substr(9, recievedCommand.size()));
		}
		else if (recievedCommand == "stop") 
		{

		}
		else if (splitCommand[0] == "go") {
			// Received command in this format: "go wtime 300000 btime 300000 winc 0 binc 0"
			//Output format: "bestmove h7h5"
			if (splitCommand.size() > 1 && (isdigit(splitCommand[1][0]) != 0))
				gameState->maxDepth = stoi(splitCommand[1]);
			gameState->board = board;
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
	}
	gameState->run = false;
	engineThread.detach();
	return;
}

//The chess engine will run here. Everything that needs to be passed to the GUI is stored in GameState
//More variables can be added to gamestate if necessary.
void runEngine(GameState* gameState)
{
	Engine engine = Engine();
	while (gameState->run)
	{
		this_thread::sleep_for(chrono::milliseconds(1));
		while (!gameState->idle)
		{
			gameState->principalVariation = engine.startSearch(gameState->board, 0, gameState->maxDepth);
			cout << "bestmove " << IO::convertMoveToAlg(gameState->principalVariation[0]) << endl;
			gameState->idle = true;
		}
	}
}

