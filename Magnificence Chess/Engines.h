#pragma once
#include "GameState.h"
#include <vector>
#include <atomic>
#include "ABAI.h"
#include "IO.h"
#include <thread>
#include "Test.h"
#include <mutex>

class Engine
{
private:
	Engine();
	~Engine();
	static void SearchThreaded(GameState &gameState, mutex beforeWork, atomic<u8> *depth, atomic<u8> *update);
	static void Killer(bool *killer, double time, atomic<bool> *change);
public:
	static vector<u32> search(GameState &gameState);
	static vector<u32> searchID(GameState &gameState);
	static vector<u32> searchIDSimpleTime(GameState &gameState);
	static vector<u32> multiThreadedSearch(GameState &gameState);
	static double calculateTimeForMove(GameState & gameState);
};

