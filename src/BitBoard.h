#pragma once
#define INTRINSICS_H_INCLUDED

#if defined(_WIN32)
#include <intrin.h>
#elif defined(__gnu_linux__) || defined(__linux__) || defined(__CYGWIN__)
#include "x86intrin.h"
#endif

#include <cstdlib>
#include <stdint.h>
#include <iostream>
#include <string>
#include <random>

#include "Board.h"

typedef unsigned long long int u64;
typedef unsigned long int u32;
typedef unsigned short int u16;
typedef signed short int i16;
typedef unsigned char u8;

using namespace std;

struct RHEntry
{
private:
	u64 key;
public:
	RHEntry()
	{
		key = 0;
	}
	RHEntry(u64 key, u8 visits)
	{
		this->setKey(key);
		this->setVisits(visits);
	}
	RHEntry(RHEntry *a)
	{
		key = a->key;
	}
	void setKey(u64 key)
	{
		this->key = (this->key & 0b1100000000000000000000000000000000000000000000000000000000000000) | (key & 0b0011111111111111111111111111111111111111111111111111111111111111);
	}
	void setVisits(u8 visists)
	{
		if (visists > 3)
			visists = 3;
		this->key = (((u64)visists) << 62) | (this->key & 0b0011111111111111111111111111111111111111111111111111111111111111);
	}
	bool operator ==(RHEntry a)
	{
		return ((this->key & 0b0011111111111111111111111111111111111111111111111111111111111111) == (a.key & 0b0011111111111111111111111111111111111111111111111111111111111111));
	}
	void operator =(RHEntry a)
	{
		this->key = a.key;
	}
	u8 fetchVisits()
	{
		return (u8)(this->key >> 62);
	}
	void operator ++()
	{
		if (this->fetchVisits() != 3)
		{
			this->key += 0b0100000000000000000000000000000000000000000000000000000000000000;
		}
		//return (RHEntry(this));
	}
	void operator --()
	{
		if (this->fetchVisits() != 0)
		{
			this->key -= 0b0100000000000000000000000000000000000000000000000000000000000000;
		}
		//return (RHEntry(this));
	}
};

struct RHEntryBucket
{
private:
	RHEntry Entry1, Entry2;
public:
	RHEntryBucket()
	{
		Entry1 = RHEntry();
		Entry2 = RHEntry();
	}
	void operator =(RHEntryBucket a)
	{
		this->Entry1 = a.Entry1;
		this->Entry2 = a.Entry2;
	}
	bool addEntry(RHEntry a)
	{
		if (Entry1 == a)
		{
			int a = Entry1.fetchVisits();
			++Entry1;
			return true;
		}
		else if (Entry2 == a)
		{
			int a = Entry2.fetchVisits();
			++Entry2;
			return true;
		}
		else if (Entry1.fetchVisits() == 0)
		{
			Entry1 = a;
			return true;
		}
		else if (Entry2.fetchVisits() == 0)
		{
			Entry2 = a;
			return true;
		}
		return false;
	}
	bool addEntry(u64 key)
	{
		RHEntry n(key, 1);
		return addEntry(n);
	}
	bool removeEntry(RHEntry a)
	{
		if (Entry1 == a)
		{
			--Entry1;
			return true;
		}
		else if (Entry2 == a)
		{
			--Entry2;
			return true;
		}
		return false;
	}
	bool removeEntry(u64 key)
	{
		RHEntry a(key, 0);
		return removeEntry(a);
	}

	//checks if it is a draw by repetition. If no bucket matches the input key it throws an exception
	//0 is not matching key
	//1 is not a draw
	//2 is a draw
	u8 draw(u64 key)
	{
		RHEntry test(key, 0);
		if (Entry1 == test)
		{
			if (Entry1.fetchVisits() == 3)
				return 2;
		}
		else if (Entry2 == test)
		{
			if (Entry2.fetchVisits() == 3)
				return 2;
		}
		else
		{
			return 0;
		}
		return 1;
	}
};

class BitBoard : public Board
{
public:
	// TTTUUUERRRRsssSSSSSSttttttFFFFFF
	struct Magic
	{
	public:
		u64 mask;
		u64 *table;
	};
	BitBoard();
	~BitBoard();
	inline void RemovePiece(u8 pos);
	inline void AddPiece(u8 pos, u8 piece);
	BitBoard(string fen);

	//The const 64 bit 1 used to avoid unpredicatable behaviour when leftshifting 1
	const u64 one = 1;

	//The universal set, used to simplify code, = ~((u64)0)
	const u64 universal = 0xffffffffffffffff;

	//Magics used for rooks
	//size = 64
	Magic *MRook;

	//Magics used for bishop
	//size = 64
	Magic *MBishop;

	//marks the row with 1s
	//1 is 0, 2 is 1, ..., 8 is 7
	//Size = 8
	u64 *rows;

	//marks the row with 0s
	//1 is 0, 2 is 1, ..., 8 is 7
	//Size = 8
	u64*nRows;

	//The zoobrist hash key of the position
	u64 zoobristKey;

	//The random numbers used to generate zoobrist key
	//indexed by:
	//position + 64 * piecenumber. There is no piece with piecenumber 6 or 13 leaving these for EP squares and such.
	//Positions 64 * 6 + 0 to 64 * 6 + 7 are used for EP square h through a, 
	//64 * 6 + 8 is for black queen rockad, 64 * 6 + 9 is for black king side rockad
	//64 * 6 + 10 is for white queen side rockad and 64 * 6 + 11 is for white king side rockad
	//64 * 6 + 12 is on for white and off for black
	//size = 960
	u64 *ElementArray;

	//marks the line with 1s
	//h is 0, g is 1, ..., a is 7
	//Size = 8
	u64 *columns;

	//marks the line 0s
	//h is 0, g is 1, ..., a is 7
	//Size = 8
	u64 *nColumns;

	//Attack set of knight from the given index
	//size = 64
	u64 *KnightSet;

	//Attack set of king from the given index
	//size = 64
	u64 *KingSet;

	//Mask used in move gen
	//Marks all bits that represent Up right, down left digagonal crossing
	//through the index position
	//size = 64
	u64 *URDL;

	//Mask used in move gen
	//Marks all bits that represent Up left, down right digagonal crossing
	//through the index position
	//size = 64
	u64 *ULDR;

	//Mask used in move gen
	//Marks all bits that represent line crossing
	//through the index position
	u64 *LR;

	//Mask used in move gen
	//Marks all bits that represent column crossing
	//through the index position
	//size = 64
	u64 *UD;

	//EP state of board, bitscan forward to find marked bit which is the EP line
	u8 EP;

	//Rockad state of board
	//Black queen side is lowest bit, mask = 0b1
	//Black king side is second lowest bit, mask = 0b10
	//White queen side is third lowest bit, mask = 0b100
	//White king side is fourth lowest bit, mask = 0b1000
	u8 rockad;

	//The piece list for the position, it has 15 items
	//0 is white kings, 1 is white queens, 2 is white bishops
	//3 is white rooks, 4 is white knights, 5 is white pawns, 6 is all white pieces
	//7 is black king, 8 is black queens, 9 is black bishops
	//10 is black rooks, 11 is black knights, 12 is black pawns, 13 is all black pieces
	//14 is a filler set used to simplify make unmake move
	u64 Pieces[15];

	//The mailbox for the position, it has 64 items
	//PieceCodes are the same as the indexes in piecelist
	//White: king 0, queen 1, bishop 2, rook 3, knight 4, pawn 5;
	//Black: king 7, queen 8, bishop 9, rook 10, knight 11, pawn 12;
	//Empty 14
	u8 mailBox[64];

	//Represents color of side to move
	//1 is white, 0 is false
	bool color;

	//Number of silent moves leading to this position
	u8 silent;

	//For function defenitions see the implementation file

	bool addRH(u64 key);
	void removeRH(u64 key);
	void SetUp();
	void SetState(string fen);
	void CalculateZoobrist();
	u32* WhiteLegalMoves(u32 *start);
	u32* WhiteQSearchMoves(u32 *start);
	u32* BlackLegalMoves(u32 *start);
	u32* BlackQSearchMoves(u32 *start);
	u32 getBaseMove();
	bool MakeMove(u32 move);
	void UnMakeMove(u32 move);
	inline u64 MagicRook(u64 piece, u64 occupancy);
	inline u64 MagicBishop(u64 piece, u64 occupancy);
	inline u32* extractWhiteMoves(u64 moves, u32 baseMove, u32 start, u32 *movesOut);
	inline u32* extractWhitePawnMoves(u64 moves, u32 baseMove, u32 start, u32 *movesOut);
	inline u32* extractBlackMoves(u64 moves, u32 baseMove, u32 start, u32 *movesOut);
	inline u32* extractBlackPawnMoves(u64 moves, u32 baseMove, u32 start, u32 *movesOut);
	bool LegailityChecker(u32 *move);
	u32 LowestValuedAttacker(u64 square, u64 baseMove, bool side);
	int SEE(u64 square);
	int SEEWrapper(u32 move);

	//a wrapper for population count
	u8 pc(u64 valeu)
	{
		#if defined(_WIN32)
		u8 returnValue = (u8)__popcnt64(valeu);
		#elif defined(__gnu_linux__) || defined(__linux__) || defined(__CYGWIN__)
		u8 returnValue = (u8)__builtin_popcount(valeu);
		#endif
		return returnValue;
	}

	inline u32 bitScanForward(u64 piece)
	{
		#if defined(_WIN32)
		u32 index;
		_BitScanForward64(&index, piece);
		return index;
		#elif defined(__gnu_linux__) || defined(__linux__) || defined(__CYGWIN__)
		return __builtin_ctz(piece);
		#endif
	}

	u64 pext(u64 occupancy, u64 mask)
	{
		return _pext_u64(occupancy, mask);
	}

	//copies the state of inserted bitboard to this bitboard
	void Copy(BitBoard *bb);
private:
	//used for magic generation
	void allVariations(u64 mask, vector<u32> positions, int index, int maxindex, vector<u64> *out);

	//Hash used to detect repetitions
	//Only the 256 lowest numbers should be accessed directly
	RHEntryBucket RepetitionHash[300];
};

