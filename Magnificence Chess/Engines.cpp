#include "stdafx.h"
#include "Engines.h"

using namespace std;

mutex talk;
clock_t times[120];
u64 maxTime;
bool timeCheck;
const bool DEBUG_OUTPUT = true;
const u32 THRD_CNT = 2;

Engine::Engine()
{
}

Engine::~Engine()
{
}

void Engine::SearchThreaded(threadedSearchData tsd)
{
	clock_t start = clock();
	ABAI searcher;
	BitBoard bb;
	bb.Copy(tsd.gameState->board);
	delete searcher.cont;
	searcher.cont = tsd.cont;
	u8 searchDepth = 0;
	bool say;
	while (*tsd.cont)
	{
		say = false;
		tsd.beforeWork->lock();
		if (*tsd.depth > tsd.maxDepth)
			break;
		if (((tsd.depth[0]) == searchDepth) || tsd.update[0] == 0)
		{
			say = true;
			(*tsd.depth) = (*tsd.depth) + 1;
			tsd.update[0] = tsd.depth[0] + 0;
			if (*tsd.depth > tsd.maxDepth)
				break;
		}
		searchDepth = *tsd.depth;
		tsd.update[0]--;
		tsd.beforeWork->unlock();
		searcher.search(searchDepth, tsd.gameState->fetchGeneration(), tsd.gameState->tt, &bb, tsd.gameState->color);
		if (say && (*tsd.cont))
		{
			if (timeCheck)
			{
				times[searchDepth] = max((u64)(clock() - start), maxTime / 12);
				if (searchDepth > 1 && (((times[searchDepth] * times[searchDepth]) / times[searchDepth - 1]) > maxTime * 1.1 && timeCheck && times[searchDepth] > maxTime / 4))
				{
					*tsd.cont = false;
					this_thread::sleep_for(chrono::milliseconds(1));
					*tsd.depth = searchDepth;
				}
			}
			cout << "Searched depth " << to_string(searchDepth) << " in " << to_string(clock() - start) << "ms" << endl;
		}
	}
	searcher.cont = new bool;
	tsd.beforeWork->unlock();
}

void Engine::Killer(bool * killer, double time, atomic<bool> *change)
{
	double start = clock();
	while (((double)clock() - start) < time * CLOCKS_PER_SEC && *change)
	{
		this_thread::sleep_for(chrono::milliseconds(5));
	}
	if (*change)
		*killer = false;
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
			highestFound = i2;
			pV[i2] = potEntry.bestMove;
			if (i == gameState.maxDepth)
			{
				PV.push_back(pV[i2]);
			}
			cout << IO::convertMoveToAlg(pV[i2]) << " ";
			gameState.board->MakeMove(pV[i2]);
		}
		//Unmake pV
		for (int i2 = highestFound; i2 >= 0; i2 -= 1)
		{
			gameState.board->UnMakeMove(pV[i2]);
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
	//gameState.tt->resetTT();

	gameState.UpdateGeneration();
	u8 generation = gameState.fetchGeneration();
	atomic<bool> *change = new atomic<bool>;
	*change = true;
	//Create the static array used for storing legal moves
	vector<u32> PV;

	ABAI search;
	bool *killer = search.cont;

	//thread thrd(Engine::Killer, killer, gameState.maxTime / 4, change);

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
	int highestDepth;
	while (runSearch && *killer)
	{
		//Do search
		search.resetNodes();
		PV.clear();
		clock_t bSearch = clock();
		score = search.search(i, generation, gameState.tt, gameState.board, gameState.color);
		clock_t timerEnd = clock();
		totalTime = (timerEnd - start) / double CLOCKS_PER_SEC;
		clock_t thisSearch = timerEnd - bSearch;
		branchingFactor = pow(search.nodes[0], 1 / (double)i);
		if (search.nodes[2] != 0)
		{
			if (thisSearch * pow(search.nodes[0], 1 / (double)i) + totalTime * CLOCKS_PER_SEC > gameState.maxTime * CLOCKS_PER_SEC)
			{
				runSearch = false;
				cout << "Time data: Taken Time " << to_string(thisSearch) << " Ratio searched " << to_string(pow(search.nodes[0], 1 / (double)i)) << " expected next time " << to_string(totalTime * search.nodes[1] / (double)search.nodes[2]) << endl;
			}
		
		}
		//Information generation
		cout << "info depth " << to_string(i) << " score cp " << to_string(score) << " pv ";
		//Find pV
		int highestDepth = 0;
		if (*killer)
		{
			BitBoard bb;
			bb.Copy(gameState.board);
			for (size_t i2 = 0; i2 < i; i2++)
			{
				UnpackedHashEntry potEntry(0, 0, 0, 0, 0, 0);
				if (!gameState.tt->getFromTT(bb.zoobristKey, &potEntry))
				{
					cout << "ERROR! Non-PV Node: " << endl;
					break;
				}
				highestDepth = i2;
				pV[i2] = potEntry.bestMove;
				PV.push_back(pV[i2]);
				cout << IO::convertMoveToAlg(pV[i2]) << " ";
				bb.MakeMove(pV[i2]);
			}
		}
		cout << endl;
		i++;
	}
	*change = false;
	if (!*killer)
		i--;
	i--;
	//thrd.join();
	highestDepth = 0;
	for (size_t i2 = 0; i2 < i; i2++)
	{
		BitBoard bb;
		bb.Copy(gameState.board);
		UnpackedHashEntry potEntry(0, 0, 0, 0, 0, 0);
		if (!gameState.tt->getFromTT(bb.zoobristKey, &potEntry))
		{
			cout << "ERROR! Non-PV Node: " << endl;
			break;
		}
		highestDepth = i2;
		pV[i2] = potEntry.bestMove;
		PV.push_back(pV[i2]);
		bb.MakeMove(pV[i2]);
	}
	int maxDepth = i;

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


vector<u32> Engine::multiThreadedSearch(GameState * gameState)
{
	timeCheck = true;
	maxTime = (u64)(gameState->maxTime * CLOCKS_PER_SEC);
	for (size_t i = 0; i < 120; i++)
	{
		times[i] = maxTime / 12;
	}
	clock_t start = clock();
	atomic<u8> *depth = new atomic<u8>, *update = new atomic<u8>;
	thread *thrds = new thread[gameState->threadCount];
	*depth = 0;
	*update = 0;
	atomic<bool> *mock = new atomic<bool>;
	mock[0] = true;
	bool *cont = new bool;
	*cont = true;
	mutex *m = new mutex;
	threadedSearchData tsd(gameState, m, depth, update, cont, 255);
	//thread killer(Engine::Killer, cont, gameState->maxTime, mock);
	for (size_t i = 0; i < gameState->threadCount - 1; i++)
	{
		thrds[i] = thread(Engine::SearchThreaded, tsd);
	}
	u8 depthM = gameState->maxDepth;
	SearchThreaded(tsd);
	for (size_t i = 0; i < gameState->threadCount - 1; i++)
	{
		thrds[i].join();
	}
	//killer.join();
	u32 *pV = new u32[*depth];
	vector<u32> PV;
	for (size_t i2 = 0; i2 < *depth; i2++)
	{
		BitBoard bb;
		bb.Copy(gameState->board);
		UnpackedHashEntry potEntry(0, 0, 0, 0, 0, 0);
		if (!gameState->tt->getFromTT(bb.zoobristKey, &potEntry))
		{
			cout << "ERROR! Non-PV Node: " << endl;
			break;
		}
		else if (i2 == 0)
			cout << endl << "Score " << to_string(potEntry.score) << "  " << " Time taken " << to_string((clock() - start) *1.0/CLOCKS_PER_SEC) << "  ";
		pV[i2] = potEntry.bestMove;
		PV.push_back(pV[i2]);
		bb.MakeMove(pV[i2]);
	}
	delete depth;
	delete update;
	delete cont;
	delete m;
	delete[] thrds;
	delete[] pV;
	return PV;
}

vector<u32> Engine::multiThreadedSearchDepth(GameState * gameState)
{
	timeCheck = false;
	clock_t start = clock();
	atomic<u8> *depth = new atomic<u8>, *update = new atomic<u8>;
	thread *thrds = new thread[gameState->threadCount];
	*depth = 0;
	*update = 0;
	bool *cont = new bool;
	*cont = true;
	mutex *m = new mutex;
	threadedSearchData tsd(gameState, m, depth, update, cont, gameState->maxDepth);
	for (size_t i = 0; i < gameState->threadCount - 1; i++)
	{
		thrds[i] = thread(Engine::SearchThreaded, tsd);
	}
	u8 depthM = gameState->maxDepth;
	SearchThreaded(tsd);
	for (size_t i = 0; i < gameState->threadCount - 1; i++)
	{
		thrds[i].join();
	}
	u32 *pV = new u32[*depth];
	vector<u32> PV;
	for (size_t i2 = 0; i2 < *depth; i2++)
	{
		BitBoard bb;
		bb.Copy(gameState->board);
		UnpackedHashEntry potEntry(0, 0, 0, 0, 0, 0);
		if (!gameState->tt->getFromTT(bb.zoobristKey, &potEntry))
		{
			cout << "ERROR! Non-PV Node: " << endl;
			break;
		}
		else if (i2 == 0)
			cout << endl << "Score " << to_string(potEntry.score) << "  " << " Time taken " << to_string(clock() - start) << "  ";
		pV[i2] = potEntry.bestMove;
		PV.push_back(pV[i2]);
		bb.MakeMove(pV[i2]);
	}
	delete depth;
	delete update;
	delete cont;
	delete m;
	delete[] thrds;
	delete[] pV;
	return PV;
}
