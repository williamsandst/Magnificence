#include "stdafx.h"
#include "Engines.h"

using namespace std;

vector<thread> threadPool;
threadedSearchData *tsdM;

mutex talk;
clock_t times[120];
u64 maxTime;
bool timeCheck;
const bool DEBUG_OUTPUT = true;
const int MAX_P_Depth = 2;
const u32 THRD_CNT = 2;

//
//Todo:
//Finish 1.0 
//	- Optimizations, fix PV output, fix debug output, fix timekeeping so no crash
//Future features:
//	Check extensions
//	Move lazyEval into makeMove
//	

Engine::Engine()
{
}

Engine::~Engine()
{
}

int Engine::SearchThreaded(threadedSearchData tsd)
{
	int score;
	clock_t start = clock();
	ABAI searcher;
	BitBoard bb;
	bb.Copy(tsd.gameState->board);
	delete searcher.cont;
	searcher.cont = tsd.cont;
	u8 searchDepth = 0;
	bool say;
	bool unlock = false;;
	while (*tsd.cont)
	{
		say = false;
		unlock = true;
		tsd.beforeWork->lock();
		if (*tsd.depth > tsd.maxDepth)
		{
			break;
		}
		if (((tsd.depth[0]) == searchDepth) || tsd.update[0] == 0)
		{
			say = true;
			(*tsd.depth) = (*tsd.depth) + 1;
			tsd.update[0] =  min(max(tsd.depth[0] + 0, 1), 4);
			if (*tsd.depth > tsd.maxDepth)
			{
				break;
			}
		}
		searchDepth = *tsd.depth;
		tsd.update[0]--;
		tsd.beforeWork->unlock();
		unlock = false;
		score = searcher.search(searchDepth, tsd.gameState->fetchGeneration(), tsd.gameState->tt, &bb, tsd.gameState->color);
		if (say && (*tsd.cont) /*talk.try_lock()*/)
		{
			BitBoard bbs;
			bbs.Copy(&bb);
			string str;
			str += "info depth " + to_string(searchDepth) + " score cp " + to_string(score) + " pv ";
			for (size_t i2 = 0; i2 < searchDepth; i2++)
			{
				UnpackedHashEntry potEntry(0, 0, 0, 0, 0, 0);
				if (!tsd.gameState->tt->getFromTT(bbs.zoobristKey, &potEntry))
				{
					cout << "ERROR! Non-PV Node: " << endl;
					break;
				}
				str += IO::convertMoveToAlg(potEntry.bestMove) + " ";
				bbs.MakeMove(potEntry.bestMove);
			}
			str += "\n";
			cout << str;
			//cout.flush();
			//talk.unlock();
		}
		if (!*searcher.cont)
			cout << "done";
	}
	*tsd.cont = false;
	searcher.cont = new bool;
	if (unlock)
	{
		tsd.beforeWork->unlock();
	}
	return score;
}

void Engine::PoolSearch()
{

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
			highestFound = i2;
			pV[i2] = potEntry.bestMove;
			if (i == gameState.maxDepth)
			{
				PV.push_back(pV[i2]);
			}
			cout << IO::convertMoveToAlg(pV[i2]) << " ";
			bb.MakeMove(pV[i2]);
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
	
	gameState.maxTime = calculateTimeForMove(gameState);
	gameState.UpdateGeneration();
	u8 generation = gameState.fetchGeneration();
	atomic<bool> *change = new atomic<bool>;
	*change = true;
	//Create the static array used for storing legal moves
	vector<u32> PV;

	ABAI search;
	bool *killer = search.cont;
	//bool *killer = new bool;
	*killer = true;

	//Reset the debug node counter
	search.resetNodes();

	//Variables used for debugging
	clock_t start = clock();

	thread thrd(Killer,killer, gameState.maxTime * 1.5, change);

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
	clock_t oldSearch = (gameState.maxTime * CLOCKS_PER_SEC) / 16;
	while (runSearch && *killer)
	{
		//Do search
		//search.resetNodes();
		PV.clear();
		clock_t bSearch = clock();
		score = search.search(i, generation, gameState.tt, gameState.board, gameState.color);
		clock_t timerEnd = clock();
		totalTime = (timerEnd - start) / double CLOCKS_PER_SEC;
		clock_t thisSearch = timerEnd - bSearch;
		if (thisSearch < 1)
			thisSearch = oldSearch;
		branchingFactor = pow(search.nodes[0], 1 / (double)i);
		if (search.nodes[2] != 0)
		{
			if ((thisSearch * thisSearch) / oldSearch > gameState.maxTime * CLOCKS_PER_SEC * 1.0 && ((totalTime * CLOCKS_PER_SEC) / ((double)gameState.maxTime * CLOCKS_PER_SEC) > 0.25))
			{
				runSearch = false;
				//cout << "Time data: Taken Time " << to_string(thisSearch) << " Ratio searched " << to_string((double) thisSearch / (double)oldSearch) << " expected next time " << to_string((thisSearch * thisSearch) / oldSearch) << endl;
				//cout << "Total time taken: " << to_string(totalTime * CLOCKS_PER_SEC) << endl;
			}
		
		}
		oldSearch = thisSearch;
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
	thrd.join();
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
		cout << to_string(search.nodes[0]) << " nodes in " << to_string((((end - start) / double CLOCKS_PER_SEC))) << " s (" 
			<< to_string(gameState.maxTime) << " s max) [" <<
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
	gameState->maxTime = Engine::calculateTimeForMove(*gameState);
	cout << "Time left " << to_string(gameState->maxTime) << "S " << "Threads " << to_string(gameState->threadCount) << " MaxTHRDS/depth " << to_string(MAX_P_Depth) << " Hash Reset " << to_string(RESET_HASH) << endl;
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
	thread killer(Engine::Killer, cont, gameState->maxTime, mock);
	for (size_t i = 0; i < gameState->threadCount - 1; i++)
	{
		thrds[i] = thread(Engine::SearchThreaded, tsd);
	}
	u8 depthM = gameState->maxDepth;
	int score = SearchThreaded(tsd);
	for (size_t i = 0; i < gameState->threadCount - 1; i++)
	{
		thrds[i].join();
	}
	killer.join();
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
		{
			if (DEBUG_OUTPUT)
			{
				cout << endl << "Score: " << to_string(score) << " at depth " << "0 " << endl;
				cout << to_string(0) << " nodes in " << to_string((((clock() - start) / double CLOCKS_PER_SEC))) << " s";/*[" <<
					to_string(searcher / (((clock() - start) / double CLOCKS_PER_SEC) * 1000000)) << " Mpos/sec]" << endl;
				cout << "Branching factor: " << pow(search.nodes[0], (float)1 / (float)gameState.maxDepth) << endl;

				cout << "Branching factors: ";
				for (size_t i = 0; i < gameState.maxDepth - 1; i++)
				{
					cout << endl << to_string(search.nodes[i]) << " / " << to_string(search.nodes[i + 1]) << " = ";
					cout << to_string(gameState.maxDepth - i) << "/" << to_string(gameState.maxDepth - i - 1) << ": " << to_string((float)search.nodes[i] / (float)search.nodes[i + 1])
						<< ", ";
				}*/
				cout << endl;
			}
			//cout << endl << "Score " << to_string(potEntry.score) << "  " << " Time taken " << to_string((clock() - start) *1.0 / CLOCKS_PER_SEC) << "  ";
		}
			
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
	cout << endl;
	return PV;
}

vector<u32> Engine::multiThreadedSearchDepth(GameState * gameState)
{
	gameState->maxTime = calculateTimeForMove(*gameState);
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
	int score = SearchThreaded(tsd);
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
		{
			cout << endl << "Score: " << to_string(score) << " at depth " << to_string(gameState->maxDepth) << endl;
			cout << to_string(0) << " nodes in " << to_string((((clock() - start) / double CLOCKS_PER_SEC))) << " s";
		}
			//cout << endl << "Score " << to_string(potEntry.score) << "  " << " Time taken " << to_string(clock() - start) << "  ";
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
	cout << endl;
	return PV;
}

double Engine::calculateTimeForMove(GameState &gameState)
{
	//An average game takes 40 moves. Thus, the first 30 moves are calculated as
	//timeLeft / currentMove
	//If only 10 moves remain, assume 10 moves always remain and divide up.
	double timeLeft = gameState.color ? gameState.whiteTime : gameState.blackTime;
	double avgMovesLeft = ((59.3 + (72830 - 2330 * gameState.ply) / (2644 + gameState.ply*(10 + gameState.ply))) / 2);
	double calculationTime = timeLeft / avgMovesLeft;
	if (calculationTime <= 1.1)
		calculationTime = 1.1;
	return ((calculationTime / 1000));
}
