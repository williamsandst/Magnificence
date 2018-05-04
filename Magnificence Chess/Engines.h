#pragma once
#include "GameState.h"
#include <vector>
#include "ABAI.h"
#include "IO.h"

class Engine
{
private:
	Engine();
	~Engine();
public:
	static vector<u32> search(GameState &gameState);
	static vector<u32> searchID(GameState &gameState);
	static vector<u32> searchIDSimpleTime(GameState &gameState);
	static vector<u32> multiThreadedSearch(GameState &gameState);
};

