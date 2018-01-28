#pragma once

#include "Board.h"
#include <cstdlib>
#include <intrin.h>

typedef unsigned long long int u64;
typedef unsigned long int u32;
typedef unsigned short int u16;
typedef unsigned char u8;

using namespace std;

class BitBoard : public Board
{
public:

	struct Magic
	{
	public:
		u64 mask;
		u64 *table;
	};
	BitBoard();
	~BitBoard();
	BitBoard(string fen);
	void SetState(string fen);
	const u64 one = 1, universal = 0xffffffffffffffff;
	Magic *MRook;
	Magic *MBishop;
	u64 *rows, *nRows;
	u64 *columns, *nColumns;
	u64 *KnightSet, KingSet[64];
	u64 *UL, *UR, *DL, *DR, *U, *D, *R, *L;
	//up left, up right, down left, down right, up, down, right, left
	u8 EP;
	//WK << 3, WQ << 2, BK << 1, BQ << 0;
	u8 rockad;
	u64 *Pieces; //(0 white king, 1 white queen, 2 white bishop, 3 white rook, 4 white knight, 5 white pawn, 6 all white, 7 black king, 8 black queen, 9 black bishop,10 black rook,11 black knight,12 black pawn,13 all black, 14 fillerSet)
	u8 *mailBox;	//piececode
					//white: king 0, queen 1, bishop 2, rook  3, knight  4, pawn 5
					//Black: king 7, queen 8, bishop 9, rook 10, knight 11, pawn 12
					//empty = 14;
	u8 silent;
	vector<u8> SilentMemory, RockadMemory, EPMemory;
	void SetUp();
	vector<u32> WhiteLegalMoves();
	vector<u32> BlackLegalMoves();
	void MakeMove(u32 move);
	void UnMakeMove(u32 move);
	u64 MagicRook(u64 piece, u64 occupancy)
	{
		u32 Index = 0;
		_BitScanForward64(&Index, piece);
		return MRook[Index].table[_pext_u64(occupancy & MRook[Index].mask, MRook[Index].mask)];
	}
	u64 MagicBishop(u64 piece, u64 occupancy)
	{
		u32 index;
		_BitScanForward64(&index, piece);
		return MBishop[index].table[_pext_u64(occupancy, MBishop[index].mask)];
	}
	inline void extractMoves(u64 moves, u32 start, vector<u32> *out)
	{
		u32 move, index;
		while (moves)
		{
			_BitScanForward64(&index, moves);
			move = start | (index << 6);
			if (mailBox[index] < 14)
			{
				move |= (mailBox[index] << 28) | 0b00001000000000000000000000000000;
			}
			else
			{
				move |= ((u32)14) << 28;
			}
			out->push_back(move);
			moves &= moves - 1;
		}
	}
	inline void extractPawnMoves(u64 moves, u32 start, vector<u32> *out)
	{
		u32 move, index;
		while (moves)
		{
			_BitScanForward64(&index, moves);
			move = start | (index << 6) | (one << 27);
			if (mailBox[index] < 14)
			{
				move |= (mailBox[index] << 28);
			}
			else
			{
				move |= ((u32)14) << 28;
			}
			if (index < 8 || index > 55)
			{
				move |= one << 26;
				for (u32 i = 1; i < 5; i++)
				{
					out->push_back(move | (i << 22));
				}
			}
			else
			{
				out->push_back(move);
			}
			moves &= moves - 1;
		}
	}
private:
	void allVariations(u64 mask, vector<u32> positions, int index, int maxindex, vector<u64> *out)
	{
		if (index == maxindex)
		{
			out->push_back(mask);
			mask = mask ^ (((u64)1) << positions[index]);
			out->push_back(mask);
		}
		else
		{
			allVariations(mask, positions, index + 1, maxindex, out);
			mask = mask ^ (((u64)1) << positions[index]);
			allVariations(mask, positions, index + 1, maxindex, out);
		}
	}
};

