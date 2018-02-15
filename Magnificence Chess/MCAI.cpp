#include "stdafx.h"
#include "MCAI.h"


Node * MCAI::BestChild(Node *parent)
{
	Node *start = parent->FirstChild, *end = parent->LastChild;
	Node *bestChild = start;
	start++;
	while (start != end)
	{
		if (start->score > bestChild->score)
		{
			bestChild = start;
		}
		start++;
	}
	return bestChild;
}

MCAI::MCAI()
{
}


MCAI::~MCAI()
{
}
