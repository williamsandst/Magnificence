#include "stdafx.h"
#include "ABAI.h"
#pragma once
#include "IO.h"


ABAI::ABAI()
{

}


ABAI::~ABAI()
{
}

void ABAI::movcpy(u32* pTarget, const u32* pSource, int n)
{
	while (n-- && (*pTarget++ = *pSource++));
}

int ABAI::insertTT(UnpackedHashEntry newEntry)
{
	PackedHashEntry entry(newEntry);
	int index = newEntry.key & hashMask;
	//if both type 0 and generation different or depth lower or 
	if ((extractNodeType(ttDepthFirst[index]) != 0 || newEntry.typeOfNode == 0) &&
		(extractDepth(ttDepthFirst[index]) <= newEntry.depth || extractGeneration(ttDepthFirst[index]) != newEntry.depth))
		ttDepthFirst[index] = entry;
	else
		ttAlwaysOverwrite[index] = entry;
	return 1;
}

bool ABAI::getFromTT(u64 key, UnpackedHashEntry *in)
{
	u32 index = key & hashMask;
	if (extractKey(ttDepthFirst[index]) == key)
	{
		*in = UnpackedHashEntry(ttDepthFirst[index]);
		return true;
	}
	else if (extractKey(ttAlwaysOverwrite[index]) == key)
	{
		*in = UnpackedHashEntry(ttAlwaysOverwrite[index]);
		return true;
	}
	else
		return false;
}

int ABAI::negamax(int alpha, int beta, int depth, int maxDepth, bool color, u32 *start, u32 *triangularPV, short pvIndex)
{
	if (depth == 0)
	{
		if (color)
			return eval();
		else
			return -eval();
	}
	else
	{
		u32 bestMove;
		short bestScore = -1000000;
		bool previousBestMove = false;
		{
			UnpackedHashEntry potEntry(0, 0, 0, 0, 0, 0);
			if (getFromTT(bb->zoobristKey, &potEntry))
			{
				if (potEntry.depth >= depth)
				{
					if (potEntry.typeOfNode == 1)
						return potEntry.score;
					else if (potEntry.typeOfNode == 0 && potEntry.score >= beta)
						return beta;
					else if (potEntry.score == 1 && potEntry.score <= alpha)
						return alpha;
				}
				bestMove = potEntry.bestMove;
				previousBestMove = true;
			}
		}
		triangularPV[pvIndex] = 0;
		u16 pvNextIndex = pvIndex + depth;
		u32 *end;
		if (color)
			end = bb->WhiteLegalMoves(start);
		else
			end = bb->BlackLegalMoves(start);
		depth--;
		color = !color;
		if (previousBestMove)
		{
			u32 *mem = start;
			while (start != end)
			{
				if (*start == bestMove)
				{
					u32 memory = *mem;
					*mem = *start;
					*start = memory;
				}
				start++;
			}
			start = mem;
		}
		while (start != end)
		{
			u32 move = *start;
			start++;
			if (move != 0 && move != 1)
			{
				bb->MakeMove(move);
				int returned = -negamax(-beta, -alpha, depth, maxDepth, color, end, triangularPV, pvNextIndex);
				if (returned > bestScore)
				{
					bestScore = returned;
					bestMove = move;
				}
				bb->UnMakeMove(move);
				if (returned >= beta)
				{
					insertTT(UnpackedHashEntry(0, depth, bestScore, bestMove, bb->zoobristKey, generation));
					return beta;
				}
				if (returned > alpha)
				{
					triangularPV[pvIndex] = move;
					movcpy(triangularPV + pvIndex + 1, triangularPV + pvNextIndex, depth);
					alpha = returned;
				}
			}
			else if (move == 1)
				return -8191 + maxDepth - depth;
			else
				return 0;
		}
		if (alpha == bestScore)
			insertTT(UnpackedHashEntry(1, depth, bestScore, bestMove, bb->zoobristKey, generation));
		else
			insertTT(UnpackedHashEntry(2, depth, bestScore, bestMove, bb->zoobristKey, generation));
		return alpha;
	}
}

int ABAI::eval()
{
	nodes++;
	return bb->pc(bb->Pieces[5]) * Pawn + bb->pc(bb->Pieces[4]) * Knight + bb->pc(bb->Pieces[3]) * Rook + bb->pc(bb->Pieces[2]) * Bishop + bb->pc(bb->Pieces[1]) * Queen
		- bb->pc(bb->Pieces[12]) * Pawn - bb->pc(bb->Pieces[11]) * Knight - bb->pc(bb->Pieces[10]) * Rook - bb->pc(bb->Pieces[9]) * Bishop - bb->pc(bb->Pieces[8]) * Queen;
}

vector<u32> ABAI::bestMove(BitBoard * IBB, bool color, clock_t time, int maxDepth)
{
	if (ttAlwaysOverwrite == nullptr)
	{	
		int size = 24 + 1; //bits
		int i = 1;
		while ((size -= 1) && (i *= 2));
		cout << i << endl;
		ttAlwaysOverwrite = new PackedHashEntry[i];
		ttDepthFirst = new PackedHashEntry[i];
		hashMask = i - 1;
	}
	generation = (generation + 1) & 0b11;
	u32 *MoveStart = new u32[218 * 100];
	clock_t start = clock();
	u32 *triangularPV = new u32[(maxDepth * (maxDepth + 1)) / 2];
	this->bb = IBB;
	nodes = 0;
	vector<u32> PV;
	u32 movePlaceHolder = 0;
	int score = negamax(-1000000, 1000000, maxDepth, maxDepth, color, MoveStart, triangularPV, 0);
	clock_t end = clock();
	cout << score << "   " << nodes << "   " << to_string(nodes / (((end - start) / double CLOCKS_PER_SEC) * 1000000)) << " Mpos/sec\n";
	for (size_t i = 0; i < maxDepth; i++)
	{
		PV.push_back(triangularPV[i]);
		cout << IO::convertMoveToAlg(triangularPV[i]) << endl;
	}
	delete[] triangularPV, MoveStart;
	return PV;
}

PackedHashEntry::PackedHashEntry(UnpackedHashEntry start)
{
	//data needs to store the best move, the score of the node, 
	//the type of node (lowest value possible value, highest possible value, exact value)
	//depth searched
	//score is in centipawns, the highest possible score is 3900 only counting piece score, it is signed. That represents 13 bits (+- 2^12 - 1 = +-4095)
	//therefore 16 bits will be assigned to score because that is a short (+-(2^ 15 - 1) = +- 32767) to allow for something unexpected
	//64 - 16 = 48;
	//type of node takes exactly 3 bits. 48 - 3 = 45;
	//The move is 32 bit and can be stored as such. 47 - 32 = 12;
	//generation is the point when the node was created. It is updated by generation = (generation + 1) & 0b11
	//generation is 2 bits. 12 - 2 = 10;
	//10 ^ 2 - 1 = 1023 which is more than sufficient for the depht.
	data = start.score | start.bestMove << 16 | start.typeOfNode << 48 | start.generation << 51 | start.generation << 53;
	key = start.key ^ data;
}

PackedHashEntry::PackedHashEntry()
{
	key = 0;
	data = 0;
}

UnpackedHashEntry::UnpackedHashEntry(u8 typeOfNode, short depth, short score, u32 bestMove, u64 key, u8 generation)
{
	this->typeOfNode = typeOfNode; //(0 is minimum value, 1 is exact, 2 is maximum value)
	this->depth = depth;
	this->score = score;
	this->bestMove = bestMove;
	this->key = key;
	this->generation = generation;
}

UnpackedHashEntry::UnpackedHashEntry(PackedHashEntry in)
{
	score = (short)(0xffff & in.data);
	bestMove = (u32)(0xffffffff & (in.data >> 16));
	typeOfNode = (u8)(((u8)0b11) & (in.data >> 48));
	generation = (u8)(((u8)0b11) & in.data >> 51);
	depth = (short)(in.data >> 53);
	key = in.key ^ in.data;
}

u8 ABAI::extractNodeType(PackedHashEntry in)
{
	return (u8)(((u8)0b11) & (in.data >> 48));
}

short ABAI::extractDepth(PackedHashEntry in)
{
	return (short)(in.data >> 53);
}

short ABAI::extractScore(PackedHashEntry in)
{
	return (short)(0xffff & in.data);
}

u32 ABAI::extractBestMove(PackedHashEntry in)
{
	return (u32)(0xffffffff & (in.data >> 16));
}

u64 ABAI::extractKey(PackedHashEntry in)
{
	return in.key ^ in.data;
}

u8 ABAI::extractGeneration(PackedHashEntry in)
{
	return (u8)(((u8)0b11) & in.data >> 51);
}
