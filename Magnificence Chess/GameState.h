#pragma once
#include <atomic>
#include <vector>
#include "BitBoard.h"

using namespace std;

class GameState
{
public:
	atomic<bool> run;
	atomic<bool> idle;
	atomic<bool> completedSearch;
	//BitBoard board;
	vector<__int16> principalVariation;
	int maxDepth = 5;
	GameState();
	~GameState();
};
