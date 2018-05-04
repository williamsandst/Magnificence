#include "stdafx.h"
#include "Engines.h"

const bool DEBUG_OUTPUT = true;

Engine::Engine()
{
}


Engine::~Engine()
{
}

vector<u32> Engine::search(GameState &gameState)
{
	//Standard search
	gameState.UpdateGeneration();
	u8 generation = gameState.fetchGeneration();

	//Variables used for debugging
	clock_t start = clock();

	ABAI search;
	search.resetNodes();
	int score = search.search(gameState.maxDepth, generation, gameState.tt, gameState.board, gameState.color);

	u32 pV[100];

	vector<u32> PV;

	cout << endl;

	clock_t end = clock();

	cout << "info depth " << to_string(gameState.maxDepth) << " score cp " << to_string(score) << " pv ";
	//Extract PV
	int maxDepth = 0;
	for (size_t i = 0; i < gameState.maxDepth; i++)
	{
		UnpackedHashEntry potEntry(0, 0, 0, 0, 0, 0);
		if (!gameState.tt->getFromTT(gameState.board->zoobristKey, &potEntry))
		{
			cout << "ERROR! Non-PV Node: " << endl;
		}
		maxDepth++;
		pV[i] = potEntry.bestMove;
		PV.push_back(pV[i]);
		cout << IO::convertMoveToAlg(pV[i]) << " ";
		gameState.board->MakeMove(pV[i]);
	}
	//Unmake pV
	for (size_t i = 1; i < maxDepth + 1; i++)
	{
		gameState.board->UnMakeMove(pV[gameState.maxDepth - i]);
	}
	cout << endl;


	if (DEBUG_OUTPUT)
	{
		cout << endl << "Score: " << to_string(score) << " at depth " << to_string(gameState.maxDepth) << endl;
		cout << to_string(search.nodes[0]) << " nodes in " << to_string((((end - start) / double CLOCKS_PER_SEC))) << " s [" <<
			to_string(search.nodes[0] / (((end - start) / double CLOCKS_PER_SEC) * 1000000)) << " Mpos/sec]" << endl;
		cout << "Branching factor: " << pow(search.nodes[0], (float)1 / (float)gameState.maxDepth) << endl;

		cout << "Branching factors: ";
		for (size_t i = 0; i < gameState.maxDepth - 1; i++)
		{
			cout << endl << to_string(search.nodes[i]) << " / " << to_string(search.nodes[i + 1]) << " = ";
			cout << to_string(gameState.maxDepth - i) << "/" << to_string(gameState.maxDepth - i - 1) << ": " << to_string((float)search.nodes[i] / (float)search.nodes[i + 1])
				<< ", ";
		}
		cout << endl;
	}

	//Memory cleanup

	return PV;
}

vector<u32> Engine::searchID(GameState &gameState)
{
	//Standard search
	gameState.UpdateGeneration();
	u8 generation = gameState.fetchGeneration();

	vector<u32> PV;

	ABAI search;
	search.resetNodes();

	//Variables used for debugging
	clock_t start = clock();

	int score;

	u32 pV[100];

	cout << endl;
	int alpha = -8192;
	int beta = 8192;
	for (int i = 1; i < gameState.maxDepth + 1; i++)
	{
		//Do search
		int highestFound = 0;
		score = search.search(i, generation, gameState.tt, gameState.board, gameState.color);
		cout << "info depth " << to_string(i) << " score cp " << to_string(score) << " pv ";
		for (size_t i2 = 0; i2 < i; i2++)
		{
			UnpackedHashEntry potEntry(0, 0, 0, 0, 0, 0);
			if (!gameState.tt->getFromTT(gameState.board->zoobristKey, &potEntry))
			{
				cout << "ERROR! Non-PV Node: " << endl;
				break;
			}
			highestFound++;
			pV[i2] = potEntry.bestMove;
			if (i == gameState.maxDepth)
			{
				PV.push_back(pV[i2]);
			}
			cout << IO::convertMoveToAlg(pV[i2]) << " ";
			gameState.board->MakeMove(pV[i2]);
		}
		//Unmake pV
		for (size_t i2 = 1; i2 < highestFound + 1; i2++)
		{
			gameState.board->UnMakeMove(pV[i - i2]);
		}
		cout << endl;
	}
	clock_t end = clock();

	if (DEBUG_OUTPUT)
	{
		cout << endl << "Score: " << to_string(score) << " at depth " << to_string(gameState.maxDepth) << endl;
		cout << to_string(search.nodes[0]) << " nodes in " << to_string((((end - start) / double CLOCKS_PER_SEC))) << " s [" <<
			to_string(search.nodes[0] / (((end - start) / double CLOCKS_PER_SEC) * 1000000)) << " Mpos/sec]" << endl;
		cout << "Branching factor: " << pow(search.nodes[0], (float)1 / (float)gameState.maxDepth) << endl;

		cout << "Branching factors: ";
		for (size_t i = 0; i < gameState.maxDepth - 1; i++)
		{
			cout << endl << to_string(search.nodes[i]) << " / " << to_string(search.nodes[i + 1]) << " = ";
			cout << to_string(gameState.maxDepth - i) << "/" << to_string(gameState.maxDepth - i - 1) << ": " << to_string((float)search.nodes[i] / (float)search.nodes[i + 1])
				<< ", ";
		}
	}

	cout << endl;

	return PV;
}

vector<u32> Engine::searchIDSimpleTime(GameState &gameState)
{
	//Standard search
	//resetTT();
	gameState.UpdateGeneration();
	u8 generation = gameState.fetchGeneration();

	//Create the static array used for storing legal moves

	BitBoard *bb = gameState.board;
	vector<u32> PV;

	ABAI search;

	//Reset the debug node counter
	search.resetNodes();

	//Variables used for debugging
	clock_t start = clock();

	int score;

	u32 pV[100];

	cout << endl;

	double totalTime;
	double branchingFactor;
	const int maxTime = 20;
	bool runSearch = true;
	cout << endl;
	int i = 1;
	while (runSearch)
	{
		//Do search
		PV.clear();
		score = search.search(i, generation, gameState.tt, gameState.board, gameState.color);
		clock_t timerEnd = clock();
		totalTime = (timerEnd - start) / double CLOCKS_PER_SEC;
		branchingFactor = pow(search.nodes[0], 1 / (double)i);
		if (totalTime * branchingFactor > gameState.maxTime)
			runSearch = false;
		//Information generation
		cout << "info depth " << to_string(i) << " score cp " << to_string(score) << " pv ";
		//Find pV
		int highestDepth = 0;
		for (size_t i2 = 0; i2 < i; i2++)
		{
			UnpackedHashEntry potEntry(0, 0, 0, 0, 0, 0);
			if (!gameState.tt->getFromTT(bb->zoobristKey, &potEntry))
			{
				cout << "ERROR! Non-PV Node: " << endl;
				break;
			}
			highestDepth++;
			pV[i2] = potEntry.bestMove;
			PV.push_back(pV[i2]);
			cout << IO::convertMoveToAlg(pV[i2]) << " ";
			bb->MakeMove(pV[i2]);
		}
		//Unmake pV
		for (size_t i2 = 1; i2 < highestDepth + 1; i2++)
		{
			bb->UnMakeMove(pV[i - i2]);
		}
		cout << endl;
		i++;
	}

	int maxDepth = i - 1;

	clock_t end = clock();

	if (DEBUG_OUTPUT)
	{
		cout << endl << "Score: " << to_string(score) << " at depth " << to_string(maxDepth) << endl;
		cout << to_string(search.nodes[0]) << " search.nodes in " << to_string((((end - start) / double CLOCKS_PER_SEC))) << " s [" <<
			to_string(search.nodes[0] / (((end - start) / double CLOCKS_PER_SEC) * 1000000)) << " Mpos/sec]" << endl;
		cout << "Branching factor: " << pow(search.nodes[0], (float)1 / (float)maxDepth) << endl;

		cout << "Branching factors: ";
		for (size_t i = 0; i < maxDepth - 1; i++)
		{
			cout << endl << to_string(search.nodes[i]) << " / " << to_string(search.nodes[i + 1]) << " = ";
			cout << to_string(maxDepth - i) << "/" << to_string(maxDepth - i - 1) << ": " << to_string((float)search.nodes[i] / (float)search.nodes[i + 1])
				<< ", ";
		}
	}

	cout << endl;

	return PV;
}
