#include "stdafx.h"
#include "ABAI.h"
#pragma once
#include "IO.h"
#include <cmath>


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

int ABAI::insertTT(PackedHashEntry newEntry)
{;
	int index = extractKey(newEntry) & hashMask;
	//if both type 0 and generation different or depth lower or 
	if ((extractNodeType(ttDepthFirst[index]) != 1 || extractNodeType(newEntry) == 1 || extractGeneration(newEntry) != extractGeneration(ttDepthFirst[index])) &&
		(extractDepth(ttDepthFirst[index]) <= extractDepth(newEntry) || extractGeneration(ttDepthFirst[index]) != extractGeneration(newEntry)))
		ttDepthFirst[index] = newEntry;
	else
		ttAlwaysOverwrite[index] = newEntry;
	return extractDepth(newEntry);
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



int ABAI::negamax(int alpha, int beta, int depth, int maxDepth, bool color, u32 *start, u16 *killerMoves)
{
	nodes[depth]++;
	if (depth == 0) return color ? lazyEval(): -lazyEval();
	u32 bestMove = 0;
	short bestScore = -8192;
	bool previousBestMove = false;

	//Check whether there is a transposition that can be used for this position
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
		}
	}

	u32 *end;
	
	//Generate legal moves for this position
	if (color)
		end = bb->WhiteLegalMoves(start);
	else
		end = bb->BlackLegalMoves(start);
	depth--;
	color = !color;

	//If a move is part of the principal variation, search that first!
	SortMoves(start, end, bestMove, killerMoves);
	
	//Go through the legal moves
	while (start != end)
	{
		u32 move = *start;
		start++;
		if (move != 0 && move != 1)
		{
			bb->MakeMove(move);
			int returned = -negamax(-beta, -alpha, depth, maxDepth, color, end, killerMoves + 2);
			if (returned > bestScore)
			{
				bestScore = returned;
				bestMove = move;
			}
			bb->UnMakeMove(move);
			if (returned >= beta)
			{
				*(killerMoves + moveOveride[depth]) = (u16)move;
				moveOveride[depth] = (moveOveride[depth] + 1) & 1;
				insertTT(UnpackedHashEntry(0, depth + 1, bestScore, bestMove, bb->zoobristKey, generation));
				return beta;
			}
			if (returned > alpha)
			{
				alpha = returned;
			}
		}
		else if (move == 1) //If in check, return a very bad score
			return -4095 + maxDepth - depth;
		else
			return 0; //If draw, return 0
	}
	//Create an entry for the transposition table
	if (alpha == bestScore)
		insertTT(UnpackedHashEntry(1, depth + 1, bestScore, bestMove, bb->zoobristKey, generation));
	else
		insertTT(UnpackedHashEntry(2, depth + 1, bestScore, bestMove, bb->zoobristKey, generation));
	return alpha;
}

int ABAI::lazyEval()
{
	//nodes[0]++;
	short score = 0;
	score += bb->pc(bb->Pieces[5]) * Pawn + bb->pc(bb->Pieces[4]) * Knight + bb->pc(bb->Pieces[3]) * Rook + bb->pc(bb->Pieces[2]) * Bishop + bb->pc(bb->Pieces[1]) * Queen
		- bb->pc(bb->Pieces[12]) * Pawn - bb->pc(bb->Pieces[11]) * Knight - bb->pc(bb->Pieces[10]) * Rook - bb->pc(bb->Pieces[9]) * Bishop - bb->pc(bb->Pieces[8]) * Queen;
	score += pieceSquareValues(whitePawnEarlyPST, bb->Pieces[5]);
	score -= pieceSquareValues(blackPawnEarlyPST, bb->Pieces[12]);
	score += pieceSquareValues(whiteKnightEarlyPST, bb->Pieces[4]);
	score -= pieceSquareValues(blackKnightEarlyPST, bb->Pieces[10]);
	return score;
}

int ABAI::pieceSquareValues(short * pieceSquareTable, u64 pieceSet)
{
	u32 index;
	short sum = 0;
	while (pieceSet)
	{
		_BitScanForward64(&index, pieceSet);
		pieceSet &= pieceSet - 1;
		sum += pieceSquareTable[index];
	}
	return sum;
}

vector<u32> ABAI::bestMove(BitBoard * IBB, bool color, clock_t time, int maxDepth)
{
	/*
	To do
	0.	Q-search
	1.	Iterative Depening
	2.	History Heurestic
	3.	Move Sorting
	4.	Multithreading
	6.	PV search
	7.	Lazy eval into make unmake move
	8.	More heuristics
	*/
	if (ttAlwaysOverwrite == nullptr)
	{	
		int size = 24 + 1; //bits
		int i = 1;
		while ((size -= 1) && (i *= 2));
		//cout << i << endl;
		ttAlwaysOverwrite = new PackedHashEntry[i];
		ttDepthFirst = new PackedHashEntry[i];
		hashMask = i - 1;
	}
	generation = (generation + 1) & 0b11;
	for (int i = 0; i < 100; i++)
	{
		if (moveOveride[i] > 1)
		{
			moveOveride[i] = 0;
		}
	}

	//Create the static array used for storing legal moves
	u32 *MoveStart = new u32[218 * 100];
	u16 *KillerMoves = new u16[200];

	//Create the triangular principal variation array

	this->bb = IBB;
	vector<u32> PV;

	//Reset the debug node counter
	for (size_t i = 0; i < 100; i++)
		nodes[i] = 0;

	//Variables used for debugging
	clock_t start = clock();

	//Run search
	int score = negamax(-8192, 8192, maxDepth, maxDepth, color, MoveStart, KillerMoves);
	
	clock_t end = clock();

	//Debug output
	cout << endl << "Score: " << to_string(score) << " at depth " << to_string(maxDepth) << endl;
	cout << to_string(nodes[0]) << " nodes in " << to_string((((end - start) / double CLOCKS_PER_SEC))) << " s [" <<
		to_string(nodes[0] / (((end - start) / double CLOCKS_PER_SEC) * 1000000)) << " Mpos/sec]" << endl;
	cout << "Branching factor: " << pow(nodes[0], (float)1 / (float)maxDepth) << endl;
	
	cout << "Branching factors: ";
	for (size_t i = 0; i < maxDepth-1; i++)
	{
		cout << endl << to_string(nodes[i]) << " / " << to_string(nodes[i + 1]) << " = ";
		cout << to_string(maxDepth - i) << "/" << to_string(maxDepth - i - 1) << ": " << to_string((float)nodes[i] / (float)nodes[i + 1])
			<< ", ";
	}
	/*cout << endl << "Principal variation (tri-pV table): ";
	for (size_t i = 0; i < maxDepth; i++)
	{
		//PV.push_back(triangularPV[i]);
		cout << IO::convertMoveToAlg(triangularPV[i]) << " ";
	}*/

	//Find pV variation from transposition table
	cout << endl << "Principal variation (TT): ";
	u32 pV[100];
	for (size_t i = 0; i < maxDepth; i++)
	{
		UnpackedHashEntry potEntry(0, 0, 0, 0, 0, 0);
		if (!getFromTT(bb->zoobristKey, &potEntry))
			cout << "ERROR: Move not part of pV? Check replacement scheme!" << endl;
		pV[i] = potEntry.bestMove;
		PV.push_back(pV[i]);
		cout << IO::convertMoveToAlg(pV[i]) << ", ";
		bb->MakeMove(pV[i]);
	}
	cout << endl;
	//Undo the pV moves
	for (size_t i = 1; i < maxDepth+1; i++)
	{
		bb->UnMakeMove(pV[maxDepth-i]);
	}

	cout << endl;

	//Memory cleanup
	delete[] MoveStart, KillerMoves;

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
	data = ((u16)start.score) | (u64)start.bestMove << 16 | (u64)start.typeOfNode << 48 | (u64)start.generation << 51 | (u64)start.depth << 54;
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
	depth = (short)(in.data >> 54);
	key = in.key ^ in.data;
}

u8 ABAI::extractNodeType(PackedHashEntry in)
{
	return (u8)(((u8)0b11) & (in.data >> 48));
}

short ABAI::extractDepth(PackedHashEntry in)
{
	return (short)(in.data >> 54);
}

short ABAI::extractScore(PackedHashEntry in)
{
	return (short)(0xffff & in.data);
}

void ABAI::SortMoves(u32 * start, u32 * end, u32 bestMove, u16 *killerMoves)
{
	u32 *OGstart = start, mem, u32, *KMStart = start, *mover;
	bestMove &= (ToFromMask);
	u16 KM1 = *killerMoves & ToFromMask, KM2 = *(killerMoves + 1) & ToFromMask;
	while (start != end)
	{
		u16 move = (*start) & ToFromMask;
		if (move == bestMove)
		{
			if (OGstart != KMStart)
			{
				mover = KMStart - 1;
				while (mover >= OGstart)
				{
					mem = *mover;
					*mover = *(mover + 1);
					*(mover + 1) = mem;
					mover--;
				}
			}
			mem = *start;
			*start = *OGstart;
			KMStart++;
			*OGstart = mem;
			OGstart++;
		}
		else if (move == KM1 || move == KM2)
		{
			mem = *start;
			*start = *KMStart;
			*KMStart = mem;
			KMStart++;
		}
		start++;
	}
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
