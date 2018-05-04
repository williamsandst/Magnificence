#include "stdafx.h"
#include "GameState.h"

GameState::GameState()
{
	generation = 0;
}

void GameState::UpdateGeneration()
{
	generation = (generation + 1) & 0b111;
}
u8 GameState::fetchGeneration()
{
	return generation;
}
;

GameState::~GameState()
{
	delete tt;
};
