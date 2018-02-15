#pragma once
#include "BitBoard.h"
#include <math.h>

struct Node
{
	Node *parent, *firstChild, *lastChild;
	u32 move, win, total;
	u8 stateOfNode;
	float score;
	bool hasGeneratedChildren;
};

class MonteCarloAI
{
private:
	int Search(BitBoard bb, Node *start, u32 *moves, bool color);
	Node* HighestNode(Node*start, Node* end);
	void DeleteTree(Node *start);
public:
	MonteCarloAI();
	~MonteCarloAI();
	vector<u32> GetBestMove(BitBoard bb, int time, bool color);
};

