#pragma once
#include <atomic>
#include "ArrayBoard.h"

using namespace std;

class GameState
{
public:
	atomic<bool> run;
	atomic<bool> idle;
	atomic<bool> completedSearch;
	ArrayBoard board;
	vector<__int16> principalVariation;
	int maxDepth = 5;
	GameState();
	~GameState();
};

