#pragma once
#include <atomic>

using namespace std;

class GameState
{
public:
	atomic<bool> run;
	atomic<bool> idle;
	atomic<bool> completedSearch;
	GameState();
	~GameState();
};

