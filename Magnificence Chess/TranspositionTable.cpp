#pragma once
#include "stdafx.h"
#include "TranspositionTable.h"

u32 TranspositionTable::extractBestMove(PackedHashEntry in)
{
	return (u32)(0xffffffff & (in.data >> 16));
}

//extracts the zoobrist hash from a packed hash entry
u64 TranspositionTable::extractKey(PackedHashEntry in)
{
	return in.key ^ in.data;
}

//extracts generation from a packed hash entry
u8 TranspositionTable::extractGeneration(PackedHashEntry in)
{
	return (u8)(((u8)0b111) & in.data >> 51);
}

//Extracts the depth from a packed hash entry
short TranspositionTable::extractDepth(PackedHashEntry in)
{
	return (short)(in.data >> 54);
}

//Extracts the score from a packed hash entry
short TranspositionTable::extractScore(PackedHashEntry in)
{
	return (short)(0xffff & in.data);
}

//extracts the type of node from a packed hash entrt
//0 is minimum value, 1 is exact value and 2 is maximum value
u8 TranspositionTable::extractNodeType(PackedHashEntry in)
{
	return (u8)(((u8)0b11) & (in.data >> 48));
}

//Generates a packed hash entry from an unpacked hash entry
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
	//generation is the point when the node was created. It is updated by generation = (generation + 1) & 0b111
	//generation is 2 bits. 12 - 3 = 9;
	//10 ^ 2 - 1 = 1023 which is more than sufficient for the depht.
	data = ((u64)((u16)start.score)) | (((u64)start.bestMove) << 16) | (((u64)start.typeOfNode) << 48) | (((u64)start.generation) << 51) | (((u64)start.depth) << 54);
	key = start.key ^ data;
}

//generates an empty packed hash entry
PackedHashEntry::PackedHashEntry()
{
	key = 0;
	data = 0;
}

//generates an unpacked hash entry
//type of node: The type of node 0 is a minimum value, 1 is an exact value, 2 is a maximum value
//Depth: the depth it was searched at
//Score: The score returned
//Bestmove: The move causing the highest score
//Key: The zoobrist hash of position
//Generation: A variable showing roughly when it was searched
UnpackedHashEntry::UnpackedHashEntry(u8 typeOfNode, u16 depth, short score, u32 bestMove, u64 key, u8 generation)
{
	this->typeOfNode = typeOfNode; //(0 is minimum value, 1 is exact, 2 is maximum value)
	this->depth = depth;
	this->score = score;
	this->bestMove = bestMove;
	this->key = key;
	this->generation = generation;
}

//Decodes a packed hash entry into a unpacked hash entry
UnpackedHashEntry::UnpackedHashEntry(PackedHashEntry in)
{
	score = (short)(0xffff & in.data);
	bestMove = (u32)(0xffffffff & (in.data >> 16));
	typeOfNode = (u8)(((u8)0b11) & (in.data >> 48));
	generation = (u8)(((u8)0b111) & in.data >> 51);
	depth = (short)(in.data >> 54);
	key = in.key ^ in.data;
}

TranspositionTable::TranspositionTable()
{
	this->ttAlwaysOverwrite = nullptr;
	this->ttDepthFirst = nullptr;
}

TranspositionTable::TranspositionTable(u8 hashSizeBits)
{
	this->hashSizeBits = hashSizeBits;
	resetTT();
}


TranspositionTable::~TranspositionTable()
{
	if (ttAlwaysOverwrite != nullptr)
	{
		delete[] ttAlwaysOverwrite;
		delete[] ttDepthFirst;
	}
}

void TranspositionTable::setHashSizeBits(u8 bits)
{
	hashSizeBits = bits;
	resetTT();
}

void TranspositionTable::resetTT()
{
	if (ttAlwaysOverwrite != nullptr)
	{
		delete[] ttAlwaysOverwrite;
		delete[] ttDepthFirst;
	}
	int size = hashSizeBits + 1; //bits
	int i = 1;
	while ((size -= 1) && (i *= 2));
	//cout << i << endl;
	ttAlwaysOverwrite = new PackedHashEntry[i];
	ttDepthFirst = new PackedHashEntry[i];
	for (size_t i = 0; i < i; i++)
	{
		ttAlwaysOverwrite[i] = PackedHashEntry();
		ttDepthFirst[i] = PackedHashEntry();
	}
	hashMask = i - 1;
}

//adds a board position to the transposition table
int TranspositionTable::insertTT(PackedHashEntry newEntry)
{
	u32 index = (u32)(extractKey(newEntry) & hashMask);
	//if both type 0 and generation different or depth lower or 
	if ((extractNodeType(ttDepthFirst[index]) != 1 || extractNodeType(newEntry) == 1 || extractGeneration(newEntry) != extractGeneration(ttDepthFirst[index])) &&
		(extractDepth(ttDepthFirst[index]) <= extractDepth(newEntry) || extractGeneration(ttDepthFirst[index]) != extractGeneration(newEntry)))
	{
		ttDepthFirst[index] = newEntry;
		if (extractKey(newEntry) == extractKey(ttAlwaysOverwrite[index]))
			ttAlwaysOverwrite[index] = PackedHashEntry();
	}
	else
	{
		if (extractKey(ttDepthFirst[index]) != extractKey(ttAlwaysOverwrite[index]))
		{
			ttAlwaysOverwrite[index] = newEntry;
		}
	}
	return extractDepth(newEntry);
}

//Checks if a board position is in the transposition table
bool TranspositionTable::getFromTT(u64 key, UnpackedHashEntry *in)
{
	u32 index = (u32)(key & hashMask);
	PackedHashEntry entry = ttDepthFirst[index];
	if (extractKey(entry) == key)
	{
		*in = UnpackedHashEntry(entry);
		return true;
	}
	entry = ttAlwaysOverwrite[index];
	if (extractKey(entry) == key)
	{
		*in = UnpackedHashEntry(entry);
		return true;
	}
	else
		return false;
}