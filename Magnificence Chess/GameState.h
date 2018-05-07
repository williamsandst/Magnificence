#pragma once
#include <atomic>
#include <vector>
#include "BitBoard.h"
#include "TranspositionTable.h"

using namespace std;

class GameState
{
private:
	u8 generation;
public:
	//Variables related to search, is changed by GUI
	int whiteTime = 600*1000;
	int blackTime = 600*1000;

	int ply = 0;

	TranspositionTable *tt;
	atomic<bool> run;
	atomic<bool> idle;
	atomic<bool> completedSearch;
	BitBoard * board;
	vector<u32> principalVariation;
	bool color = true;
	int maxDepth = 0;
	int threadCount = 1;
	double maxTime;
	GameState();
	void UpdateGeneration();
	u8 fetchGeneration();
	~GameState();
};
