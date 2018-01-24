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
	const bool CONSOLEDEBUG = true;
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
	//Standard board;
	board = IO::convertFENtoArrayBoard("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
	//board = IO::convertFENtoArrayBoard("8/8/8/3Kr3/8/3N4/3r4/8 w - - 0 1");
	//board = IO::convertFENtoArrayBoard("8/8/8/3Kr3/3P4/8/3r4/8 w - - 0 1");
	//board = IO::convertFENtoArrayBoard("k6K/8/8/8/1p1p1p1p/8/PPPPPPPP/8 w - - 0 1");
	//board = IO::convertFENtoArrayBoard("8/2ppp3/8/3K4/8/2ppp3/8/8 w - - 0 1");
	//Wrong at ply 2! Above FEN String, probably king related
	//board = IO::convertFENtoArrayBoard("K7/2Q1Q1Q1/8/2Q1k1Q1/8/2Q1Q1Q1/8/b7 w - - 0 1");
	//board = IO::convertFENtoArrayBoard("k2q4/8/8/3R4/q1RKR2r/3R4/8/3r4 w - - 0 1");
	//Kiwipete perft test
	//board = IO::convertFENtoArrayBoard("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1");
	//Third perft test
	//board = IO::convertFENtoArrayBoard("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1");
	//Fourth perft test
	//board = IO::convertFENtoArrayBoard("r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1");
	
	//board = IO::convertFENtoArrayBoard("8/6R1/8/8/6B1/8/1Q6/6P1 w - - 0 1");
	while (getline(cin, recievedCommand))
	{
		vector<string> splitCommand = split(recievedCommand, ' ');
		if (CONSOLEDEBUG) //Debug commands, not used for interface!
		{
			if (splitCommand[0] == "test" || splitCommand[0] == "t")
			{
				if (splitCommand[1] == "perftdivide" || splitCommand[1] == "perftdiv" || splitCommand[1] == "pdiv")
				{
					vector<string> dividedMoves = Test::perftDivide(board, stoi(splitCommand[2]));
					long totalCount = 0;
					for (size_t i = 0; i < dividedMoves.size(); i++)
					{
						cout << dividedMoves[i] << endl;
						totalCount += stoi(split(dividedMoves[i], ' ')[1]);
					}
					cout << "Total count: " << totalCount << endl;

				}
				if (splitCommand[1] == "perft")
				{
					timer = clock();
					unsigned long perftNumber = Test::recursivePerft(board, stoi(splitCommand[2]));
					duration = (clock() - timer) / (double)CLOCKS_PER_SEC;
					cout << perftNumber << " [" << to_string(duration) << " s] " << "[" << to_string((perftNumber/1000000)/duration) << " MN/S]" << endl;
				}
				if (splitCommand[1] == "fen")
				{
					cout << IO::convertBoardToFEN(board) << endl;
				}
				if (splitCommand[1] == "display" || splitCommand[1] == "disp")
				{
					cout << Test::displayBoard(board);
				}
				if (splitCommand[1] == "move")
				{
					board.makeMove(IO::convertAlgToMove(splitCommand[2]));
					board.whiteTurn = !board.whiteTurn;
				}
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
				if (splitCommand[1] == "legalmoves" || splitCommand[1] == "lmov")
				{
					if (splitCommand.size() > 2)
					{
						if (splitCommand[2] == "white" || splitCommand[2] == "w")
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
						else if (splitCommand[2] == "black" || splitCommand[2] == "b")
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
		else if (splitCommand[0] == "setboard")
		{
			board = IO::convertFENtoArrayBoard(recievedCommand.substr(9, recievedCommand.size()));
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

