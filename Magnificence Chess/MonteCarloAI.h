#pragma once
#include "BitBoard.h"

class MonteCarloAI
{
	struct Node
	{
		Node *parent, *firstChild, *lastChild;
		bool hasGeneratedChildren;
	};
public:
	MonteCarloAI();
	~MonteCarloAI();
};

