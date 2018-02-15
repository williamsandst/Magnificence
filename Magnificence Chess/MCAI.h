#pragma once
#include "BitBoard.h"

struct Node
{
	u32 move, win, visit;
	float score;
	Node *FirstChild, *LastChild;
};
class MCAI
{
private:
	Node * BestChild(Node *parent);
public:
	MCAI();
	~MCAI();
};

