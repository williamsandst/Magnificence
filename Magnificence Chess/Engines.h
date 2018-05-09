#pragma once
#include "GameState.h"
#include <vector>
#include <atomic>
#include "ABAI.h"
#include "IO.h"
#include <thread>
#include "Test.h"
#include <mutex>

struct threadedSearchData
{
	threadedSearchData(GameState *in, mutex *m, atomic<u8> *depth, atomic<u8> *update, bool*cont, u8 maxdepth)
	{
		gameState = in;
		beforeWork = m;
		this->depth = depth;
		this->update = update;
		this->cont = cont;
		this->maxDepth = maxdepth;
	};
	GameState *gameState;
	mutex *beforeWork;
	atomic<u8> *depth;
	atomic<u8> *update;
	bool *cont;
	u8 maxDepth;
};

class Engine
{
private:
	Engine();
	~Engine();
	static void Killer(bool *killer, double time, atomic<bool> *change);
public:
	static int SearchThreaded(threadedSearchData tsd);
	static vector<u32> search(GameState &gameState);
	static vector<u32> searchID(GameState &gameState);
	static vector<u32> searchIDSimpleTime(GameState &gameState);

	static vector<u32> multiThreadedSearch(GameState *gameState);
	static vector<u32> multiThreadedSearchDepth(GameState *gameState);

	static double calculateTimeForMove(GameState & gameState);

};

