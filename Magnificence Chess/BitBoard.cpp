#include "stdafx.h"
#include "BitBoard.h"
#include "Board.h"


BitBoard::BitBoard()
{
	SetUp();
}


BitBoard::~BitBoard()
{
}

void BitBoard::SetUp()
{
	MRook = new Magic[64];
	MBishop = new Magic[64];
	rows = new u64[8];
	nRows = new u64[8];
	nColumns = new u64[8];
	columns = new u64[8];
	KingSet = new u64[64];
	KnightSet = new u64[64];
	Pieces = new u64[15];
	mailBox = new u8[64];
	UL = new u64[64];
	UR = new u64[64];
	DL = new u64[64];
	DR = new u64[64];
	U = new u64[64];
	D = new u64[64];
	R = new u64[64];
	L = new u64[64];
	//creating rows and columns tables
	for (int i = 0; i < 8; i++)
	{
		columns[i] = one << i;
		columns[i] = columns[i] << 8 | columns[i];
		columns[i] = columns[i] << 16 | columns[i];
		columns[i] = columns[i] << 32 | columns[i];
		rows[i] = one << (i * 8);
		rows[i] = rows[i] << 1 | rows[i];
		rows[i] = rows[i] << 2 | rows[i];
		rows[i] = rows[i] << 4 | rows[i];
		nRows[i] = ~rows[i];
		nColumns[i] = ~columns[i];
	}
	//Generate King and knight sets + magics + vectors
	for (int i = 0; i < 64; i++)
	{
		//Generate King and knight sets
		{
			u64 start = one << i;
			KingSet[i] = (((start << 1) | (start << 9) | (start >> 7) | (start << 8) | (start >> 8)) & ~(columns[0]))
				| (((start >> 1) | (start >> 9) | (start << 7)) & (~columns[7]));
			KnightSet[i] = (((start << 10) | (start >> 6)) & ~(columns[0] | columns[1])) | (((start << 17) | (start >> 15)) & (~(columns[0])))
				| (((start << 6) | (start >> 10)) & ~(columns[7] | columns[6])) | (((start << 15) | (start >> 17)) & ~(columns[7]));
		};
		//generate RookTables
		//check RookTables
		{
			u32 index;
			u64 pos = one << i, mask;
			mask = ((rows[i / 8] ^ pos) & (~(columns[0] ^ columns[7])));
			mask |= ((columns[i % 8] ^ pos) & (~(rows[0] ^ rows[7])));
			u64 attacks = 0, pattern = 0, count = 0, memory = mask;
			vector<u64> out;
			vector<u32> indexes;
			while (memory)
			{
				count++;
				_BitScanForward64(&index, memory);
				memory = memory & (memory - 1);
				indexes.push_back(index);
			}
			allVariations(mask, indexes, 0, count - 1, &out);
			int size = out.size();
			u64 *ptr = new u64[size];
			for (int i2 = 0; i2 < size; i2++)
			{
				u64 ocupancy = out[i2];
				{
					pos = one << i;
					attacks = 0;
					while (pos)
					{
						pos = pos << 8;
						attacks |= pos;
						pos = pos & (~ocupancy);
					}
					pos = one << i;
					while (pos)
					{
						pos = pos >> 8;
						attacks |= pos;
						pos = pos & (~ocupancy);
					}
					pos = one << i;
					while (pos)
					{
						pos = (pos >> 1) & (~columns[7]);
						attacks |= pos;
						pos = pos & (~ocupancy);
					}
					pos = one << i;
					while (pos)
					{
						pos = (pos << 1) & (~columns[0]);
						attacks |= pos;
						pos = pos & (~ocupancy);
					}
				};
				ptr[_pext_u64(ocupancy, mask)] = attacks;
			}
			for (int i2 = 0; i2 < size; i2++)
			{
				u64 ocupancy = out[i2];
				{
					pos = one << i;
					attacks = 0;
					while (pos)
					{
						pos = pos << 8;
						attacks |= pos;
						pos = pos & (~ocupancy);
					}
					pos = one << i;
					while (pos)
					{
						pos = pos >> 8;
						attacks |= pos;
						pos = pos & (~ocupancy);
					}
					pos = one << i;
					while (pos)
					{
						pos = (pos >> 1) & (~columns[7]);
						attacks |= pos;
						pos = pos & (~ocupancy);
					}
					pos = one << i;
					while (pos)
					{
						pos = (pos << 1) & (~columns[0]);
						attacks |= pos;
						pos = pos & (~ocupancy);
					}
				};
				if (ptr[_pext_u64(ocupancy, mask)] != attacks)
				{
					int q = 5;
				}
				else
				{
					MRook[i].mask = mask;
					MRook[i].table = ptr;
				};
			}
		};
		//Generate BishopTables
		//check BishopTables
		{
			u32 index;
			u64 pos, mask = 0;
			pos = one << i;
			while (pos)
			{
				pos = (pos << 9) & (~(columns[7] | rows[7] | columns[0]));
				mask |= pos;
			}
			pos = one << i;
			while (pos)
			{
				pos = (pos >> 7) & (~(columns[7] | rows[0] | columns[0]));
				mask |= pos;
			}
			pos = one << i;
			while (pos)
			{
				pos = (pos >> 9) & (~(columns[0] | rows[0] | columns[7]));
				mask |= pos;
			}
			pos = one << i;
			while (pos)
			{
				pos = (pos << 7) & (~(columns[0] | rows[7] | columns[7]));
				mask |= pos;
			}
			u64 attacks = 0, pattern = 0, count = 0, memory = mask;
			vector<u64> out;
			vector<u32> indexes;
			while (memory)
			{
				count++;
				_BitScanForward64(&index, memory);
				memory = memory & (memory - 1);
				indexes.push_back(index);
			}
			allVariations(mask, indexes, 0, count - 1, &out);
			int size = out.size();
			u64 *ptr = new u64[size];
			for (int i2 = 0; i2 < size; i2++)
			{
				u64 ocupancy = out[i2];
				attacks = 0;
				{
					pos = one << i;
					while (pos)
					{
						pos = (pos << 9) & (~columns[0]);
						attacks |= pos;
						pos = pos & (~ocupancy);
					}
					pos = one << i;
					while (pos)
					{
						pos = (pos >> 7) & (~columns[0]);
						attacks |= pos;
						pos = pos & (~ocupancy);
					}
					pos = one << i;
					while (pos)
					{
						pos = (pos << 7) & (~columns[7]);
						attacks |= pos;
						pos = pos & (~ocupancy);
					}
					pos = one << i;
					while (pos)
					{
						pos = (pos >> 9) & (~columns[7]);
						attacks |= pos;
						pos = pos & (~ocupancy);
					}
				}
				ptr[_pext_u64(ocupancy, mask)] = attacks;
			}
			for (int i2 = 0; i2 < size; i2++)
			{
				u64 ocupancy = out[i2];
				attacks = 0;
				{
					pos = one << i;
					while (pos)
					{
						pos = (pos << 9) & (~columns[0]);
						attacks |= pos;
						pos = pos & (~ocupancy);
					}
					pos = one << i;
					while (pos)
					{
						pos = (pos >> 7) & (~columns[0]);
						attacks |= pos;
						pos = pos & (~ocupancy);
					}
					pos = one << i;
					while (pos)
					{
						pos = (pos << 7) & (~columns[7]);
						attacks |= pos;
						pos = pos & (~ocupancy);
					}
					pos = one << i;
					while (pos)
					{
						pos = (pos >> 9) & (~columns[7]);
						attacks |= pos;
						pos = pos & (~ocupancy);
					}
				}
				if (ptr[_pext_u64(ocupancy, mask)] != attacks)
				{
					int q = 5;
				}
				else
				{
					MBishop[i].mask = mask;
					MBishop[i].table = ptr;
				};
			}
		};
		//Generate vectors
		{
			u64 pos = one << i;
			U[i] = 0;
			while (pos)
			{
				pos = pos << 8;
				U[i] |= pos;
			}
			pos = one << i;
			D[i] = 0;
			while (pos)
			{
				pos = pos >> 8;
				D[i] |= pos;
			}
			pos = one << i;
			L[i] = 0;
			while (pos)
			{
				pos = (pos << 1) & (~columns[0]);
				L[i] |= pos;
			}
			pos = one << i;
			R[i] = 0;
			while (pos)
			{
				pos = (pos >> 1) & nColumns[7];
				R[i] |= pos;
			}
			pos = one << i;
			DR[i] = 0;
			while (pos)
			{
				pos = (pos >> 9) & nColumns[7];
				DR[i] |= pos;
			}
			pos = one << i;
			UR[i] = 0;
			while (pos)
			{
				pos = (pos << 7) & nColumns[7];
				UR[i] |= pos;
			}
			pos = one << i;
			UL[i] = 0;
			while (pos)
			{
				pos = (pos << 9) & (~columns[0]);
				UL[i] |= pos;
			}
			pos = one << i;
			DL[i] = 0;
			while (pos)
			{
				pos = (pos >> 7) & (~columns[0]);
				DL[i] |= pos;
			}
		}
	}
}
//Piece taken 4, ein pasant 1, rockad 1, upgrade 1, upgrade to 4, to 6, from 6
//Ein Pasant state 8
//move encoding TTTTNUBBBBRESSSSSSSSttttttffffff
//Piece taken [4] = T ( << 28), non silent [1] = N (<< 27), upgrade [1] = U (<<26)
//Upgrade to [4] = B ( << 22), Rockad [1] = R ( << 21), einpassant [1] = E ( << 20);
//Ein Passant state [8] = S ( << 12), to [6] = t (<< 6), from[6] = f (<< 0);
//0 = patt;
//universal = matt

vector<u32> BitBoard::WhiteLegalMoves()
{
	vector<u32> result;
	result.reserve(30);
	u32 kingIndex;
	u64 occupancy = Pieces[6] | Pieces[13], PinnedPieces = 0,
		legalTargets = 0, ownPieces = Pieces[6];
	int checks = 0;
	//Find checks and pinned pieces and king moves
	{
		//pawns
		u64 ThreatenedSquaresO = ((Pieces[12] >> 9) & nColumns[7]) | ((Pieces[12] >> 7) & nColumns[0]);
		u64 EPiece = Pieces[11], king = Pieces[0];
		u32 index;
		//Knights
		while (EPiece)
		{
			_BitScanForward64(&index, EPiece);
			EPiece &= EPiece - 1;
			ThreatenedSquaresO |= KnightSet[index];
		}
		//king
		_BitScanForward64(&index, Pieces[7]);
		ThreatenedSquaresO |= KingSet[index];
		if (ThreatenedSquaresO & king)
		{
			checks++;
		}
		//rooks + queens
		EPiece = Pieces[8] | Pieces[10];
		_BitScanForward64(&kingIndex, king);
		u64 kingAttacks = MagicRook(king, occupancy);
		u64 NKOccupancy = occupancy ^ king;
		u64 memory = EPiece & (U[kingIndex]);
		EPiece = EPiece ^ memory;
		while (memory)
		{
			u64 ret = MagicRook(memory, NKOccupancy);
			ThreatenedSquaresO |= ret;
			if (ret & king)
			{
				legalTargets |= kingAttacks & U[kingIndex];
				checks++;
			}
			else
			{
				ret = MagicRook(memory, occupancy);
				PinnedPieces |= ret & kingAttacks & U[kingIndex];
			}
			memory &= memory - 1;
		}
		memory = EPiece & (D[kingIndex]);
		EPiece = EPiece ^ memory;
		while (memory)
		{
			u64 ret = MagicRook(memory, NKOccupancy);
			ThreatenedSquaresO |= ret;
			if (ret & king)
			{
				legalTargets |= kingAttacks & D[kingIndex];
				checks++;
			}
			else
			{
				ret = MagicRook(memory, occupancy);
				PinnedPieces |= ret & kingAttacks & D[kingIndex];
			}
			memory &= memory - 1;
		}
		memory = EPiece & (L[kingIndex]);
		EPiece = EPiece ^ memory;
		while (memory)
		{
			u64 ret = MagicRook(memory, NKOccupancy);
			ThreatenedSquaresO |= ret;
			if (ret & king)
			{
				legalTargets |= kingAttacks & L[kingIndex];
				checks++;
			}
			else
			{
				ret = MagicRook(memory, occupancy);
				PinnedPieces |= ret & kingAttacks & L[kingIndex];
			}
			memory &= memory - 1;
		}
		memory = EPiece & (R[kingIndex]);
		EPiece = EPiece ^ memory;
		while (memory)
		{
			u64 ret = MagicRook(memory, NKOccupancy);
			ThreatenedSquaresO |= ret;
			if (ret & king)
			{
				legalTargets |= kingAttacks & R[kingIndex];
				checks++;
			}
			else
			{
				ret = MagicRook(memory, occupancy);
				PinnedPieces |= ret & kingAttacks & R[kingIndex];
			}
			memory &= memory - 1;
		}
		while (EPiece)
		{
			ThreatenedSquaresO |= MagicRook(EPiece, NKOccupancy);
			EPiece &= EPiece - 1;
		}
		//bishops + queens
		EPiece = Pieces[8] | Pieces[9];
		kingAttacks = MagicBishop(king, occupancy);
		memory = EPiece & UL[kingIndex];
		EPiece = EPiece ^ memory;
		while (memory)
		{
			u64 ret = MagicBishop(memory, NKOccupancy);
			ThreatenedSquaresO |= ret;
			if (ret & king)
			{
				legalTargets |= kingAttacks & UL[kingIndex];
				checks++;
			}
			else
			{
				ret = MagicRook(memory, occupancy);
				PinnedPieces |= ret & kingAttacks & UL[kingIndex];
			}
			memory &= memory - 1;
		}
		memory = EPiece & UR[kingIndex];
		EPiece = EPiece ^ memory;
		while (memory)
		{
			u64 ret = MagicBishop(memory, NKOccupancy);
			ThreatenedSquaresO |= ret;
			if (ret & king)
			{
				legalTargets |= kingAttacks & UR[kingIndex];
				checks++;
			}
			else
			{
				ret = MagicRook(memory, occupancy);
				PinnedPieces |= ret & kingAttacks & UR[kingIndex];
			}
			memory &= memory - 1;
		}
		memory = EPiece & DR[kingIndex];
		EPiece = EPiece ^ memory;
		while (memory)
		{
			u64 ret = MagicBishop(memory, NKOccupancy);
			ThreatenedSquaresO |= ret;
			if (ret & king)
			{
				legalTargets |= kingAttacks & DR[kingIndex];
				checks++;
			}
			else
			{
				ret = MagicRook(memory, occupancy);
				PinnedPieces |= ret & kingAttacks & DR[kingIndex];
			}
			memory &= memory - 1;
		}
		memory = EPiece & DL[kingIndex];
		EPiece = EPiece ^ memory;
		while (memory)
		{
			u64 ret = MagicBishop(memory, NKOccupancy);
			ThreatenedSquaresO |= ret;
			if (ret & king)
			{
				legalTargets |= kingAttacks & DL[kingIndex];
				checks++;
			}
			else
			{
				ret = MagicRook(memory, occupancy);
				PinnedPieces |= ret & kingAttacks & DL[kingIndex];
			}
			memory &= memory - 1;
		}
		while (EPiece)
		{
			ThreatenedSquaresO |= MagicBishop(EPiece, NKOccupancy);;
			EPiece &= EPiece - 1;
		}
		u64 kingMoves = KingSet[kingIndex] & (~ThreatenedSquaresO) & (~ownPieces);
		extractMoves(kingMoves, kingIndex, &result);
		if (checks > 1)
		{
			return result;
		}
		else if (checks == 0)
		{
			if ((rockad & 8) && !((14) & ThreatenedSquaresO))
			{
				u32 move = 59 | (57 << 6) | one << 21 | (((u64)EP) << 12);
			}
			if ((rockad & 4) && !((56) & ThreatenedSquaresO))
			{
				u32 move = 59 | (57 << 6) | one << 21 | (((u64)EP) << 12);
			}
		}
	}
	return result;
}

vector<u32> BitBoard::BlackLegalMoves()
{
	vector<u32> result;
	result.reserve(30);
	u32 kingIndex;
	u64 occupancy = Pieces[6] | Pieces[13], PinnedPieces = 0,
		legalTargets = 0, ownPieces = Pieces[13];
	int checks = 0;
	//Find checks and pinned pieces
	{
		//pawns
		u64 ThreatenedSquaresO = ((Pieces[5] << 7) & nColumns[7]) | ((Pieces[5] << 9) & nColumns[0]);
		u64 EPiece = Pieces[11], king = Pieces[7];
		u32 index;
		//Knight
		while (EPiece)
		{
			_BitScanForward64(&index, EPiece);
			EPiece &= EPiece - 1;
			ThreatenedSquaresO |= KnightSet[index];
		}
		//king
		_BitScanForward64(&index, Pieces[0]);
		ThreatenedSquaresO |= KingSet[index];
		if (ThreatenedSquaresO & king)
		{
			checks++;
		}
		//queen + rooks
		EPiece = Pieces[1] | Pieces[3];
		_BitScanForward64(&kingIndex, king);
		u64 kingAttacks = MagicRook(king, occupancy);
		u64 NKOccupancy = occupancy ^ king;
		u64 memory = EPiece & (U[kingIndex]);
		EPiece = EPiece ^ memory;
		while (memory)
		{
			u64 ret = MagicRook(memory, NKOccupancy);
			ThreatenedSquaresO |= ret;
			if (ret & king)
			{
				legalTargets |= kingAttacks & U[kingIndex];
				checks++;
			}
			else
			{
				ret = MagicRook(memory, occupancy);
				PinnedPieces |= ret & kingAttacks & U[kingIndex];
			}
			memory &= memory - 1;
		}
		memory = EPiece & (D[kingIndex]);
		EPiece = EPiece ^ memory;
		while (memory)
		{
			u64 ret = MagicRook(memory, NKOccupancy);
			ThreatenedSquaresO |= ret;
			if (ret & king)
			{
				legalTargets |= kingAttacks & D[kingIndex];
				checks++;
			}
			else
			{
				ret = MagicRook(memory, occupancy);
				PinnedPieces |= ret & kingAttacks & D[kingIndex];
			}
			memory &= memory - 1;
		}
		memory = EPiece & (L[kingIndex]);
		EPiece = EPiece ^ memory;
		while (memory)
		{
			u64 ret = MagicRook(memory, NKOccupancy);
			ThreatenedSquaresO |= ret;
			if (ret & king)
			{
				legalTargets |= kingAttacks & L[kingIndex];
				checks++;
			}
			else
			{
				ret = MagicRook(memory, occupancy);
				PinnedPieces |= ret & kingAttacks & L[kingIndex];
			}
			memory &= memory - 1;
		}
		memory = EPiece & (R[kingIndex]);
		EPiece = EPiece ^ memory;
		while (memory)
		{
			u64 ret = MagicRook(memory, NKOccupancy);
			ThreatenedSquaresO |= ret;
			if (ret & king)
			{
				legalTargets |= kingAttacks & R[kingIndex];
				checks++;
			}
			else
			{
				ret = MagicRook(memory, occupancy);
				PinnedPieces |= ret & kingAttacks & R[kingIndex];
			}
			memory &= memory - 1;
		}
		while (EPiece)
		{
			ThreatenedSquaresO |= MagicRook(EPiece, NKOccupancy);
			EPiece &= EPiece - 1;
		}
		//Queen + bishops
		EPiece = Pieces[1] | Pieces[2];
		kingAttacks = MagicBishop(king, occupancy);
		memory = EPiece & UL[kingIndex];
		EPiece = EPiece ^ memory;
		while (memory)
		{
			u64 ret = MagicBishop(memory, NKOccupancy);
			ThreatenedSquaresO |= ret;
			if (ret & king)
			{
				legalTargets |= kingAttacks & UL[kingIndex];
				checks++;
			}
			else
			{
				ret = MagicRook(memory, occupancy);
				PinnedPieces |= ret & kingAttacks & UL[kingIndex];
			}
			memory &= memory - 1;
		}
		memory = EPiece & UR[kingIndex];
		EPiece = EPiece ^ memory;
		while (memory)
		{
			u64 ret = MagicBishop(memory, NKOccupancy);
			ThreatenedSquaresO |= ret;
			if (ret & king)
			{
				legalTargets |= kingAttacks & UR[kingIndex];
				checks++;
			}
			else
			{
				ret = MagicRook(memory, occupancy);
				PinnedPieces |= ret & kingAttacks & UR[kingIndex];
			}
			memory &= memory - 1;
		}
		memory = EPiece & DR[kingIndex];
		EPiece = EPiece ^ memory;
		while (memory)
		{
			u64 ret = MagicBishop(memory, NKOccupancy);
			ThreatenedSquaresO |= ret;
			if (ret & king)
			{
				legalTargets |= kingAttacks & DR[kingIndex];
				checks++;
			}
			else
			{
				ret = MagicRook(memory, occupancy);
				PinnedPieces |= ret & kingAttacks & DR[kingIndex];
			}
			memory &= memory - 1;
		}
		memory = EPiece & DL[kingIndex];
		EPiece = EPiece ^ memory;
		while (memory)
		{
			u64 ret = MagicBishop(memory, NKOccupancy);
			ThreatenedSquaresO |= ret;
			if (ret & king)
			{
				legalTargets |= kingAttacks & DL[kingIndex];
				checks++;
			}
			else
			{
				ret = MagicRook(memory, occupancy);
				PinnedPieces |= ret & kingAttacks & DL[kingIndex];
			}
			memory &= memory - 1;
		}
		while (EPiece)
		{
			ThreatenedSquaresO |= MagicBishop(EPiece, NKOccupancy);;
			EPiece &= EPiece - 1;
		}
		u64 kingMoves = KingSet[kingIndex] & (~ThreatenedSquaresO) & (~ownPieces);
		u64 kingMoves = KingSet[kingIndex] & (~ThreatenedSquaresO) & (~ownPieces);
		extractMoves(kingMoves, kingIndex, &result);
		if (checks > 1)
		{
			return result;
		}
		else if (checks == 0)
		{
			if ((rockad & 2) && !((((u64)14) << 56) & ThreatenedSquaresO))
			{
				u32 move = 59 | (57 << 6) | one << 21 | (((u64)EP) << 12);
			}
			if ((rockad & 1) && !((((u64)56) << 56) & ThreatenedSquaresO))
			{
				u32 move = 59 | (57 << 6) | one << 21 | (((u64)EP) << 12);
			}
		}
	}
	return result;
}

void BitBoard::MakeMove(u32 move)
{
}

void BitBoard::UnMakeMove(u32 move)
{
}
