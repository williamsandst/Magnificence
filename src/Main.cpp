// Magnificence Chess.cpp : Defines the entry point for the console application.
//

#include <iostream>
#include <thread>
#include <sstream>
#include <chrono>
#include <atomic>
#include <ctime>

#include "ABAI.h"
#include "Board.h"
#include "GameState.h"
#include "BitBoard.h"
#include "IO.h"
#include "Move.h"
#include "Test.h"
#include "Engine.h"

string commandList =
"\nCommand list for Magnificence Chess Engine Development Build \n"
"disp				Display the board\n"
"go					Engine searches for a move\n"
"move <move>			Perform a move\n"
"perft <depth>			Calculate perft for current position\n"
"hperft <depth>			Perft with hashing\n"
"mperft <depth>			Multithreaded perft using Lazy SMP\n"
"divide <depth>			Divide the perft on first depth for debugging\n"
"moves <color>			Display legal moves at current position\n"
"ttreset				Resets the transposition table\n"
"setboard <fen>			Set the board to FEN position\n"
"fen				Output a fen string for current position\n"
"uci				Enables uci-mode and gives control to a GUI\n";

using namespace std;

static const int threadCount = 1;

void runEngine(GameState* gameState, ABAI* engine);
void guiInterface();

int main()
{
	guiInterface();
    return 0;
}

//This function recieves input from a separate GUI in the form of UCI commands.
void guiInterface()
{
	//Settings
	bool CONSOLEDEBUG = true;
	//Create engine thread object
	GameState* gameState = new GameState();
	ABAI* engine = new ABAI();
	
	engine->resetTT();
	gameState->idle = true;
	gameState->run = true;
	gameState->maxTime = 4;
	thread engineThread(runEngine, gameState, engine);


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
			else if (splitCommand[0] == "selfplay")
			{
				engine->selfPlay(stoi(splitCommand[1]), stoi(splitCommand[2]), gameState);
			}
			else if (splitCommand[0] == "hperft" && splitCommand.size() == 2)
			{
				//tablesize should be power of 2 - 1;
				u32 *start = new u32[218 * (stoi(splitCommand[1]) + 1)];
				u32 tableSize = 16777215;//8388607;
				HashEntryPerft *Hash = new HashEntryPerft[2 * tableSize + 2];
				timer = clock();
				bool done = true;
				u64 perftNumber = Test::perftHash(stoi(splitCommand[1]), stoi(splitCommand[1]), &board, color, start, Hash, tableSize, &done);
				HashEntryPerft *thisPos = (Hash + (((board.zoobristKey & tableSize) * 2)));
				u64 perftNumber2 = thisPos->GetResult();
				duration = (clock() - timer) / (double)CLOCKS_PER_SEC;
				cout << perftNumber << " [" << to_string(duration) << " s] " << "[" << to_string((perftNumber / 1000000.0F) / duration) << " MN/S]" << endl;
				delete[] start;
				delete[] Hash;
			}
			else if (splitCommand[0] == "perft" && splitCommand.size() == 2)
			{
				//tablesize should be power of 2 - 1;
				u32 *start = new u32[218 * (stoi(splitCommand[1]) + 1)];
				timer = clock();
				u64 perftNumber = Test::perft(stoi(splitCommand[1]), &board, color, start);
				duration = (clock() - timer) / (double)CLOCKS_PER_SEC;
				cout << perftNumber << " [" << to_string(duration) << " s] " << "[" << to_string((perftNumber / 1000000.0F) / duration) << " MN/S]" << endl;
				delete[] start;
			}
			else if (splitCommand[0] == "mperft" && splitCommand.size() >= 2)
			{
				u32 **starts = new u32*[threadCount];
				for (int i = 0; i < threadCount; i++)
				{
					starts[i] = new u32[218 * (stoi(splitCommand[1]) + 1)];
				}
				u32 tableSize = 16777215;//8388607;
				HashEntryPerft *Hash = new HashEntryPerft[2 * tableSize + 2];
				timer = clock();
				BitBoard threadBoard[threadCount];
				for (size_t i = 0; i < threadCount; i++)
				{
					threadBoard[i].Copy(&board);
				}
				vector<thread> perftThreads(threadCount - 1);
				bool done = true;
				for (size_t i = 0; i < threadCount - 1; i++)
				{
					perftThreads[i] = thread(Test::perftHash, stoi(splitCommand[1]), stoi(splitCommand[1]), &threadBoard[i], color, starts[i], Hash, tableSize, &done);
					this_thread::sleep_for(0.001s);
				}
				Test::perftHash(stoi(splitCommand[1]), stoi(splitCommand[1]), &threadBoard[threadCount-1], color, starts[threadCount-1], Hash, tableSize, &done);
				for (size_t i = 0; i < threadCount - 1; i++)
				{
					perftThreads[i].join();
				}
				HashEntryPerft *thisPos = (Hash + (((board.zoobristKey & tableSize) * 2)));
				u64 perftNumber = thisPos->GetResult();
				duration = (clock() - timer) / (double)CLOCKS_PER_SEC;
				cout << perftNumber << " [" << to_string(duration) << " s] " << "[" << to_string((perftNumber / 1000000.0F) / duration) << " MN/S]" << endl;
				delete[] Hash;
				for (int i = 0; i < threadCount; i++)
				{
					delete[] starts[i];
				}
				delete[] starts;
			}
			else if (splitCommand[0] == "movesort" && splitCommand.size() == 2)
			{
				if (splitCommand[1] == "white")
				{
					u32 *start = new u32[218];
					i16 *sortingScores = new i16[218];
					u32 *end = board.WhiteLegalMoves(start);

					u16 killerMoves[2] = { 0,0 };

					ABAI tempEngine = ABAI();
					tempEngine.bb = &board;
					//tempEngine.calculateSortingValues(start, end, 0, killerMoves, sortingScores);

					for (size_t i = 0; i < (u32)(end - start); i++)
					{
						cout << IO::convertMoveToAlg(start[i]) << " [" <<
							Test::pieceToString(board.mailBox[Move::getTo(start[i])]) << " to " <<
							Test::pieceToString(board.mailBox[Move::getFrom(start[i])]) << "] " << 
							to_string(sortingScores[i]) << endl;
					}
					delete[]start;
				}
				else if (splitCommand[1] == "black")
				{
					u32 *start = new u32[218];
					i16 *sortingScores = new i16[218];
					u32 *end = board.BlackLegalMoves(start);

					u16 killerMoves[2] = { 0,0 };

					ABAI tempEngine = ABAI();
					tempEngine.bb = &board;
					//tempEngine.calculateSortingValues(start, end, 0, killerMoves, sortingScores);

					for (size_t i = 0; i < (u32)(end - start); i++)
					{
						cout << IO::convertMoveToAlg(start[i]) << " [" <<
							Test::pieceToString(board.mailBox[Move::getTo(start[i])]) << " to " <<
							Test::pieceToString(board.mailBox[Move::getFrom(start[i])]) << "] " <<
							to_string(sortingScores[i]) << endl;
					}
					delete[]start;
				}
			}
			else if (splitCommand[0] == "ttreset" || splitCommand[0] == "resettt")
			{
				engine->resetTT();
			}
			else if ((splitCommand[0] == "testsuite" || splitCommand[0] == "ts") && splitCommand.size() == 2)
			{
				if (splitCommand[1] == "LCT2" || splitCommand[1] == "lct2")
				{
					Test::LCT2();
				}
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
							Test::pieceToString(board.mailBox[Move::getTo(start[i])]) << " to " <<
							Test::pieceToString(board.mailBox[Move::getFrom(start[i])]) << "]" << endl;
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
							Test::pieceToString(board.mailBox[Move::getTo(moves[i])]) << " to " <<
							Test::pieceToString(board.mailBox[Move::getFrom(moves[i])]) << "]" << endl;
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
			color = board.color;
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
}

//The chess engine will run here. Everything that needs to be passed to the GUI is stored in GameState
//More variables can be added to gamestate if necessary.
void runEngine(GameState* gameState, ABAI *engine)
{
	//Engine engine = Engine();
	while (gameState->run)
	{
		this_thread::sleep_for(chrono::milliseconds(2));
		while (!gameState->idle)
		{
			BitBoard localBB;
			localBB.Copy(gameState->board);
			gameState->principalVariation = engine->searchID(*gameState);
			cout << "bestmove " << IO::convertMoveToAlg(gameState->principalVariation[0]) << endl;
			//cout << "mgnf: ";
			gameState->idle = true;
		}
	}
}