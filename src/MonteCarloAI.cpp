#include "MonteCarloAI.h"


int MonteCarloAI::Search(BitBoard bb, Node *start, u32 *moves, bool color)
{
	if (start->stateOfNode == 0)
	{
		start->total++;
		return -1;
	}
	else if (start->stateOfNode == 1)
	{
		start->win++;
		start->total++;
		return 0;
	}
	else
	{
		if (!start->hasGeneratedChildren)
		{
			start->hasGeneratedChildren = 1;
			if (bb.silent >= 50)
			{
				start->stateOfNode = 1;
				return 0;
			}
			u32 *end, *current = moves;
			if (color)
			{
				end = bb.WhiteLegalMoves(moves);
			}
			else
			{
				end = bb.BlackLegalMoves(moves);
			}
			int numbeChild = end - moves;
			start->firstChild = new Node[numbeChild];
			Node *currentChild = start->firstChild;
			start->lastChild = currentChild + numbeChild;
			while (current != end)
			{
				if (*current == 1)
				{
					start->stateOfNode = 0;
					return -1;
				}
				else if (*current == 0)
				{
					start->stateOfNode = 1;
					return 0;
				}
				else
				{
					currentChild->hasGeneratedChildren = false;
					currentChild->win = 0;
					currentChild->move = *current;
					currentChild->score = 0;
					currentChild->stateOfNode = 2;
					currentChild->parent = start;
					currentChild->total = 1;
					if (currentChild == start->lastChild)
					{
						current++;
					}
					currentChild++;
					current++;
				}
			}
		}
		else
		{
			start->total++;
		}
		Node *bestChild = HighestNode(start->firstChild, start->lastChild);
		int res;
		if (bestChild->move != 0 && bestChild->move != 1)
		{
			bb.MakeMove(bestChild->move);
			cout << to_string(bb.silent) << " ";
			res = -Search(bb, bestChild, moves, !color);
			bb.UnMakeMove(bestChild->move);
		}
		else
		{
			res = -Search(bb, bestChild, moves, !color);
		}
		if (res == 1)
		{
			start->win += 5;
		}
		else if (res == 0)
		{
			start->win += 1;
		}
		Node *current = start->firstChild;
		while (current != start->lastChild)
		{
			current->score = (current->win / ((float)current->total * 5)) + 1.41f * std::sqrt(std::log(start->total) / current->total);
			current++;
		}
		return res;
	}
}

Node* MonteCarloAI::HighestNode(Node * start, Node * end)
{
	Node *best = start;
	start++;
	while (start != end)
	{
		if (start->score > best->score)
		{
			best = start;
		}
		start++;
	}
	return best;
}

void MonteCarloAI::DeleteTree(Node * start)
{
	if (start->stateOfNode == 1 || start->stateOfNode == 2)
	{
		delete[]start->firstChild;
	}
	else
	{
		Node *current = start->firstChild;
		while (current != start->lastChild)
		{
			DeleteTree(current);
			current++;
		}
		delete start->lastChild;
	}
}

MonteCarloAI::MonteCarloAI()
{
}


MonteCarloAI::~MonteCarloAI()
{
}

vector<u32> MonteCarloAI::GetBestMove(BitBoard bb, int simulations, bool color)
{
	Node *FatherNode = new Node();
	FatherNode->parent = nullptr;
	FatherNode->stateOfNode = 3;
	u32 *moves = new u32[218];
	FatherNode->hasGeneratedChildren = 0;
	for (int i = 0; i < simulations; i++)
	{
		Search(bb, FatherNode, moves, color);
	}
	Node *current = FatherNode, *bestNode, *searchNode;
	vector<u32> PV;
	while (current->stateOfNode != 1 && current->stateOfNode != 0)
	{
		searchNode = current->firstChild + 1;
		bestNode = current->firstChild;
		while (searchNode != current->lastChild)
		{
			searchNode->score = (searchNode->win / ((float)searchNode->total * 5));
			if (searchNode->score > bestNode->score)
			{
				bestNode = searchNode;
			}
			searchNode++;
		}
		current = bestNode;
		PV.push_back(bestNode->move);
	}
	DeleteTree(FatherNode);
	delete[] moves;
	return PV;
}
