#include "stdafx.h"
#include "BitBoard.h"
#include "Board.h"
#include <iostream>
#include <string>

using namespace std;

BitBoard::BitBoard()
{
	SetUp();
}


BitBoard::~BitBoard()
{
}

inline void BitBoard::RemovePiece(u8 pos)
{
	u8 removed = mailBox[pos];
	mailBox[pos] = 14;
	Pieces[removed] &= (~(one << pos));
	if (removed < 6)
	{
		Pieces[6] &= (~(one << pos));
	}
	else if (removed < 13)
	{
		Pieces[13] &= (~(one << pos));
	}
}

inline void BitBoard::AddPiece(u8 pos, u8 piece)
{
	u8 removed = mailBox[pos];
	mailBox[pos] = piece;
	Pieces[removed] &= (~(one << pos));
	if (removed < 6)
	{
		Pieces[6] &= (~(one << pos));
	}
	else if (removed < 13)
	{
		Pieces[13] &= (~(one << pos));
	}
	Pieces[piece] |= (one << pos);
	if (piece < 6)
	{
		Pieces[6] |= (one << pos);
	}
	else if (piece < 13)
	{
		Pieces[13] |= (one << pos);
	}
}

inline u64 BitBoard::MagicRook(u64 piece, u64 occupancy)
{
	u32 Index;
	_BitScanForward64(&Index, piece);
	return MRook[Index].table[_pext_u64(occupancy, MRook[Index].mask)];
}

inline u64 BitBoard::MagicBishop(u64 piece, u64 occupancy)
{
	u32 index;
	_BitScanForward64(&index, piece);
	return MBishop[index].table[_pext_u64(occupancy, MBishop[index].mask)];
}

inline u32* BitBoard::extractWhiteMoves(u64 moves, u32 baseMove, u32 start, u32 *movesOut)
{
	u32 move, BaseMove = baseMove | start, index;
	while (moves)
	{
		_BitScanForward64(&index, moves);
		move = BaseMove | (index << 6) | (((mailBox[index]) - 7) << 29);
		*movesOut = move;
		movesOut++;
		moves &= moves - 1;
	}
	return movesOut;
}

inline u32* BitBoard::extractBlackMoves(u64 moves, u32 baseMove, u32 start, u32 *movesOut)
{
	u32 move, BaseMove = baseMove | start , index;
	while (moves)
	{
		_BitScanForward64(&index, moves);
		if (mailBox[index] < 14)
		{
			move = BaseMove| (index << 6) | (((mailBox[index])) << 29);
		}
		else
		{
			move = BaseMove | (index << 6) | ((u32)0b111 << 29);
		}
		*movesOut = move;
		movesOut++;
		moves &= moves - 1;
	}
	return movesOut;
}

inline u32* BitBoard::extractBlackPawnMoves(u64 moves, u32 baseMove, u32 start, u32 *movesOut)
{
	u32 move, BaseMove = baseMove | start, index;
	while (moves)
	{
		_BitScanForward64(&index, moves);
		move = BaseMove | (index << 6);
		if (mailBox[index] < 14)
		{
			move |= (mailBox[index] << 29);
		}
		else
		{
			move |= ((u32)7) << 29;
		}
		if (index < 8 || index > 55)
		{
			for (u32 i = 1; i < 5; i++)
			{
				*movesOut = (move | (i << 26));
				movesOut++;
			}
		}
		else
		{
			*movesOut = move;
			movesOut++;
		}
		moves &= moves - 1;
	}
	return movesOut;
}

inline u32* BitBoard::extractWhitePawnMoves(u64 moves, u32 baseMove, u32 start, u32 *movesOut)
{
	u32 move, BaseMove = baseMove | start, index;
	while (moves)
	{
		_BitScanForward64(&index, moves);
		move = BaseMove | (index << 6) | ((mailBox[index] - 7) << 29);
		if (index < 8 || index > 55)
		{
			for (u32 i = 1; i < 5; i++)
			{
				*movesOut = move | (i << 26);
				movesOut++;
			}
		}
		else
		{
			*movesOut = move;
			movesOut++;
		}
		moves &= moves - 1;
	}
	return movesOut;
}

void BitBoard::allVariations(u64 mask, vector<u32> positions, int index, int maxindex, vector<u64>* out)
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

BitBoard::BitBoard(string fen)
{
	SetUp();
	SetState(fen);
}

void BitBoard::SetState(string fen)
{
	int i = 0;
	int pos = 0;
	//pieces
	while (i < fen.size() && fen[i] != ' ')
	{
		switch (fen[i])
		{
		case 'r':
			mailBox[pos] = 10;
			pos++;
			break;
		case 'n':
			mailBox[pos] = 11;
			pos++;
			break;
		case 'b':
			mailBox[pos] = 9;
			pos++;
			break;
		case 'q':
			mailBox[pos] = 8;
			pos++;
			break;
		case 'p':
			mailBox[pos] = 12;
			pos++;
			break;
		case 'k':
			mailBox[pos] = 7;
			pos++;
			break;
		case 'R':
			mailBox[pos] = 3;
			pos++;
			break;
		case 'N':
			mailBox[pos] = 4;
			pos++;
			break;
		case 'B':
			mailBox[pos] = 2;
			pos++;
			break;
		case 'Q':
			mailBox[pos] = 1;
			pos++;
			break;
		case 'P':
			mailBox[pos] = 5;
			pos++;
			break;
		case 'K':
			mailBox[pos] = 0;
			pos++;
			break;
		case '1':
			for (int i = 0; i < 1; i++)
			{
				mailBox[pos] = 14;
				pos++;
			}
			break;
		case '2':
			for (int i = 0; i < 2; i++)
			{
				mailBox[pos] = 14;
				pos++;
			}
			break;
		case '3':
			for (int i = 0; i < 3; i++)
			{
				mailBox[pos] = 14;
				pos++;
			}
			break;
		case '4':
			for (int i = 0; i < 4; i++)
			{
				mailBox[pos] = 14;
				pos++;
			}
			break;
		case '5':
			for (int i = 0; i < 5; i++)
			{
				mailBox[pos] = 14;
				pos++;
			}
			break;
		case '6':
			for (int i = 0; i < 6; i++)
			{
				mailBox[pos] = 14;
				pos++;
			}
			break;
		case '7':
			for (int i = 0; i < 7; i++)
			{
				mailBox[pos] = 14;
				pos++;
			}
			break;
		case '8':
			for (int i = 0; i < 8; i++)
			{
				mailBox[pos] = 14;
				pos++;
			}
			break;
		default:
			break;
		}
		i++;
	};
	i++;
	//turn
	while (i < fen.size() && fen[i] != ' ')
	{
		i++;
	}
	i++;
	for (int i = 0; i < 64; i++)
	{
		u8 value = mailBox[i];
		if (i > 47)
		{
			u8 jfasdf = mailBox[i];
		}
	}
	//rockad rights
	rockad = 0;
	while (i < fen.size() && fen[i] != ' ')
	{
		switch (fen[i])
		{
		case 'K':
			rockad |= 8;
			break;
		case 'k':
			rockad |= 2;
			break;
		case 'Q':
			rockad |= 4;
			break;
		case 'q':
			rockad |= 1;
			break;
		default:
			break;
		}
		i++;
	}
	i++;
	//Ein Passant
	EP = 0;
	if (i << fen.size())
	{
		switch (fen[i])
		{
		case 'a':
			EP = 128;
			break;
		case 'b':
			EP = 64;
			break;
		case 'c':
			EP = 32;
			break;
		case 'd':
			EP = 16;
			break;
		case 'e':
			EP = 8;
			break;
		case 'f':
			EP = 4;
			break;
		case 'g':
			EP = 2;
			break;
		case 'h':
			EP = 1;
			break;
		default:
			break;
		}
	}
	//Find next thing
	while (i < fen.size() && fen[i] != ' ')
	{
		i++;
	}
	i++;
	while (i < fen.size() && fen[i] != ' ')
	{
		i++;
	}
	i--;
	//silent moves
	u8 mult = 1;
	silent = 0;
	while (i < fen.size() && fen[i] != ' ')
	{
		switch (fen[i])
		{
		case '0':
			break;
		case '1':
			silent += 1 * mult;
			break;
		case '2':
			silent += 2 * mult;
			break;
		case '3':
			silent += 3 * mult;
			break;
		case '4':
			silent += 4 * mult;
			break;
		case '5':
			silent += 5 * mult;
			break;
		case '6':
			silent += 6 * mult;
			break;
		case '7':
			silent += 7 * mult;
			break;
		case '8':
			silent += 8 * mult;
			break;
		case '9':
			silent += 9 * mult;
			break;
		default:
			break;
		}
		mult = 10;
		i--;
	}
	for (int i = 0; i < 32; i++)
	{
		u8 mem = mailBox[i];
		mailBox[i] = mailBox[63 - i];
		mailBox[63 - i] = mem;
	}
	for (int i = 0; i < 15; i++)
	{
		Pieces[i] = 0;
	}
	for (int i = 0; i < 64; i++)
	{
		Pieces[mailBox[i]] |= one << i;
	}
	for (int i = 0; i < 6; i++)
	{
		Pieces[6] |= Pieces[i];
	}
	for (int i = 7; i < 13; i++)
	{
		Pieces[13] |= Pieces[i];
	}
}

void BitBoard::SetUp()
{
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
		URDL[i] = 0;
		ULDR[i] = 0;
		LR[i] = 0;
		UD[i] = 0;
		//Generate King and knight sets
		{
			u64 start = one << i;
			KingSet[i] = start << 8 | start >> 8;
			KingSet[i] |= (start << 1 | start << 9 | start >> 7) & nColumns[0];
			KingSet[i] |= (start >> 1 | start >> 9 | start << 7) & nColumns[7];
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
			while (pos)
			{
				pos = pos << 8;
				UD[i] |= pos;
			}
			pos = one << i;
			while (pos)
			{
				pos = pos >> 8;
				UD[i] |= pos;
			}
			pos = one << i;
			while (pos)
			{
				pos = (pos << 1) & (~columns[0]);
				LR[i] |= pos;
			}
			pos = one << i;
			while (pos)
			{
				pos = (pos >> 1) & nColumns[7];
				LR[i] |= pos;
			}
			pos = one << i;
			while (pos)
			{
				pos = (pos >> 9) & nColumns[7];
				ULDR[i] |= pos;
			}
			pos = one << i;
			while (pos)
			{
				pos = (pos << 7) & nColumns[7];
				URDL[i] |= pos;
			}
			pos = one << i;
			while (pos)
			{
				pos = (pos << 9) & (~columns[0]);
				ULDR[i] |= pos;
			}
			pos = one << i;
			while (pos)
			{
				pos = (pos >> 7) & (~columns[0]);
				URDL[i] |= pos;
			}
		}
	}
}
//Piece taken 4, ein pasant 1, rockad 1, upgrade 1, upgrade to 4, to 6, from 6
//Ein Pasant state 8
//move encoding					TTTTNUBBBBRE00000000ttttttffffff
// extracting special moves	  0b00001000000000000000000000000000
//								TTTUUUERRRRsssSSSSSSttttttFFFFFF
//							  0b00000100000000000000000000000000
//							  0b00000000001000000000000000000000
//Piece taken [4] = T ( << 28), non silent [1] = N (<< 27), upgrade [1] = U (<<26)
//Upgrade to [4] = B ( << 22), Rockad [1] = R ( << 21), einpassant [1] = E ( << 20);
//to [6] = t (<< 6), from[6] = f (<< 0);
//0 = patt;
//universal = matt

//in order to store entire state 4 bit rockad, 3 bit EP, 6 bit silent = 13 bits; Would need to free another 5 bits;
//1 from taken, 1 from upgrade to, 1 rockad, 1 non silent, 1 upgrade
//Taken 3 = T ( << 29), upgradeTo [3] = U << 26, Bit rockad [4] = R << 22, EP state [4] = s << 18, Silent [6] = S << 12, To [6] t << 6, From[6] == F << 6;
// TTTUUUERRRRsssSSSSSSttttttFFFFFF
u32* BitBoard::WhiteLegalMoves(u32 *Start)
{
	u32 *MoveInsert = Start;
	u32 baseMove;
	{
		u32 index;
		if (EP)
		{
			_BitScanForward(&index, EP);
		}
		else
		{
			index = 8;
		}
		baseMove = index << 18 | rockad << 22 | silent << 12;
	}
	u32 kingIndex;
	u64 occupancy = Pieces[6] | Pieces[13], PinnedPieces = 0,
		legalTargets = 0, ownPieces = Pieces[6], king = Pieces[0],
		nOccupancy = ~occupancy;
	int checks = 0;
	if (king == 0)
	{
		*MoveInsert = 1;
		MoveInsert++;
		return MoveInsert;
	}
	//Find checks and pinned pieces and king moves
	{
		//pawns
		u64 EPiece = Pieces[11];
		u64 ThreatenedSquaresO = ((Pieces[12] >> 9) & nColumns[7]);
		if (ThreatenedSquaresO & king)
		{
			checks++;
			legalTargets |= (ThreatenedSquaresO & king) << 9;
		}
		ThreatenedSquaresO |= ((Pieces[12] >> 7) & nColumns[0]);
		if (ThreatenedSquaresO & king && checks == 0)
		{
			checks++;
			legalTargets |= (ThreatenedSquaresO & king) << 7;
		}
		u32 index;
		//Knights
		while (EPiece)
		{
			_BitScanForward64(&index, EPiece);
			EPiece &= EPiece - 1;
			ThreatenedSquaresO |= KnightSet[index];
			if (KnightSet[index] & king)
			{
				checks++;
				legalTargets |= one << index;
			}
		}
		//king
		_BitScanForward64(&index, Pieces[7]);
		ThreatenedSquaresO |= KingSet[index];
		//rooks + queens
		EPiece = Pieces[8] | Pieces[10];
		_BitScanForward64(&kingIndex, king);
		u64 kingAttacks = MagicRook(king, occupancy);
		u64 NKOccupancy = occupancy ^ king;
		u64 memory = EPiece & kingAttacks;
		EPiece &= ~(kingAttacks);
		if (memory)
		{
			u64 ret = MagicRook(memory, NKOccupancy), normal = MagicRook(memory, occupancy);
			ThreatenedSquaresO |= ret;
			legalTargets |= (kingAttacks & normal) | memory;
			checks++;
		}
		if (kingIndex > 64)
		{
			cout << king;
		}
		memory = EPiece & (UD[kingIndex] | LR[kingIndex]);
		EPiece &= ~(UD[kingIndex] | LR[kingIndex]);
		while (memory)
		{
			u64 normal = MagicRook(memory, NKOccupancy);
			ThreatenedSquaresO |= normal;
			PinnedPieces |= normal & kingAttacks;
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
		memory = EPiece & kingAttacks;
		EPiece &= ~(kingAttacks);
		if (memory)
		{
			u64 ret = MagicBishop(memory, NKOccupancy), normal = MagicBishop(memory, occupancy);
			ThreatenedSquaresO |= ret;
			legalTargets |= (kingAttacks & normal) | memory;
			checks++;
		}
		memory = EPiece & (ULDR[kingIndex] | URDL[kingIndex]);
		EPiece &= ~(ULDR[kingIndex] | URDL[kingIndex]);
		while (memory)
		{
			u64 normal = MagicBishop(memory, NKOccupancy);
			ThreatenedSquaresO |= normal;
			PinnedPieces |= normal & kingAttacks;
			memory &= memory - 1;
		}
		while (EPiece)
		{
			ThreatenedSquaresO |= MagicBishop(EPiece, NKOccupancy);;
			EPiece &= EPiece - 1;
		}
		u64 kingMoves = KingSet[kingIndex] & (~ThreatenedSquaresO) & (~ownPieces);
		MoveInsert = extractWhiteMoves(kingMoves, baseMove, kingIndex, MoveInsert);
		if (checks > 1)
		{
			if (MoveInsert == Start)
			{
				*MoveInsert = 1;
				MoveInsert++;
			}
			return MoveInsert;
		}
		else if (checks == 0)
		{
			legalTargets = universal ^ ownPieces;
			if ((rockad & 8) && !((14) & ThreatenedSquaresO) && !(6 & occupancy))
			{
				u32 move = 3 | (1 << 6) | baseMove | ((u32)7) << 29;
				*MoveInsert = move;
				MoveInsert++;
			}
			if ((rockad & 4) && !((56) & ThreatenedSquaresO) && !(112 & occupancy))
			{
				u32 move = 3 | (5 << 6) | baseMove | ((u32)7) << 29;
				*MoveInsert = move;
				MoveInsert++;
			}
		}
	}; 
	//Pawn Move
	{
		u64 pawns = Pieces[5] & PinnedPieces, EPieces = Pieces[13];
		u32 index;
		while (pawns)
		{
			u64 res;
			_BitScanForward64(&index, pawns);
			pawns &= pawns - 1;
			if (ULDR[index] & king)
			{
				res = (((one << (index + 9)) & nColumns[0]) & EPieces);
			}
			else if (URDL[index] & king)
			{
				res = (((one << (index + 7)) & nColumns[7]) & EPieces);
			}
			else if (UD[index] & king)
			{
				res = (((one << (index + 8))) & nOccupancy);
				if (res & rows[2])
				{
					res |= ((res << 8) & nOccupancy);
				}
			}
			else
			{
				res = 0;
			}
			res &= legalTargets;
			MoveInsert = extractWhitePawnMoves(res, baseMove, index, MoveInsert);
		}
		pawns = ((Pieces[5] & ~(PinnedPieces)) << 9) & nColumns[0] & EPieces & legalTargets;
		while (pawns)
		{
			_BitScanForward64(&index, pawns);
			pawns &= pawns - 1;
			if (index < 56)
			{
				*MoveInsert = baseMove | index << 6 | (index - 9) | (((u32)mailBox[index] - 7) << 29);
				MoveInsert++;
			}
			else
			{
				u32 move = baseMove | index << 6 | (index - 9) | ((u32)(mailBox[index] - 7)) << 29;
				for (int i = 1; i < 5; i++)
				{
					*MoveInsert = move | (i << 26);
					MoveInsert++;
				}
			}
		}
		pawns = ((Pieces[5] & ~(PinnedPieces)) << 7) & nColumns[7] & EPieces & legalTargets;
		while (pawns)
		{
			_BitScanForward64(&index, pawns);
			pawns &= pawns - 1;
			if (index < 56)
			{
				*MoveInsert = baseMove | index << 6 | (index - 7) | (u32)(mailBox[index] - 7) << 29;
				MoveInsert++;
			}
			else
			{
				u32 move = baseMove | index << 6 | (index - 7) | (mailBox[index] - 7) << 29;
				for (int i = 1; i < 5; i++)
				{
					*MoveInsert = move | (i << 26);
					MoveInsert++;
				}
			}
		}
		pawns = ((Pieces[5] & ~(PinnedPieces)) << 8) & nOccupancy;
		u64 mem = ((pawns & rows[2]) << 8) & nOccupancy & legalTargets;
		pawns &= legalTargets;
		while (pawns)
		{
			_BitScanForward64(&index, pawns);
			pawns &= pawns - 1;
			if (index < 56)
			{
				*MoveInsert = baseMove | (index << 6) | (index - 8) | 7 << 29;
				MoveInsert++;
			}
			else
			{
				u32 move = baseMove | index << 6 | (index - 8) | 7 << 29;
				for (int i = 1; i < 5; i++)
				{
					*MoveInsert = move | (i << 26);
					MoveInsert++;
				}
			}
		}
		pawns = mem;
		while (pawns)
		{
			_BitScanForward64(&index, pawns);
			pawns &= pawns - 1;
			*MoveInsert = baseMove | index << 6 | (index - 16) | 7 << 29;
			MoveInsert++;
		}
		{
			u64 ep = ((u64)EP) << 32;
			if (ep & legalTargets)
			{
				ep <<= 8;
				pawns = ((Pieces[5] << 9) & nColumns[0] & ep);
			}
			else
			{
				ep <<= 8;
				pawns = ((Pieces[5] << 9) & nColumns[0] & ep & legalTargets);
			}
			if (pawns)
			{
				_BitScanForward64(&index, pawns);
				u64 mockOccupancy = (~((one << (index - 9)) | (one << (index - 8)))) & occupancy;
				mockOccupancy |= one << index;
				if (!((MagicRook(king, mockOccupancy) & (Pieces[8] | Pieces[10])) || 
					(MagicBishop(king, mockOccupancy) & (Pieces[8] | Pieces[9]))))
				{
					*MoveInsert = baseMove | (index - 9) | (index << 6) | 7 << 29;
					MoveInsert++;
				}
			}
			ep >>= 8;
			if (ep & legalTargets)
			{
				ep <<= 8;
				pawns = ((Pieces[5] << 7) & nColumns[7] & ep);
			}
			else
			{
				ep <<= 8;
				pawns = ((Pieces[5] << 7) & nColumns[7] & ep & legalTargets);
			}
			if (pawns)
			{
				_BitScanForward64(&index, pawns);
				u64 mockOccupancy = (~((one << (index - 7)) | (one << (index - 8)))) & occupancy;
				mockOccupancy |= one << index;
				if (!((MagicRook(king, mockOccupancy) & (Pieces[8] | Pieces[10])) ||
					(MagicBishop(king, mockOccupancy) & (Pieces[8] | Pieces[9]))))
				{
					*MoveInsert = baseMove | (index - 7) | (index << 6) | 7 << 29;
					MoveInsert++;
				}
			}
		}
	};
	//Queen + Bishop Move
	{
		u32 index;
		u64 qb = (Pieces[1] | Pieces[2]) & PinnedPieces;
		while (qb)
		{
			u64 res;
			_BitScanForward64(&index, qb);
			if ((ULDR[index]) & king)
			{
				res = MagicBishop(qb, occupancy) & (ULDR[index]) & legalTargets;
				MoveInsert = extractWhiteMoves(res, baseMove, index, MoveInsert);
			}
			else if ((URDL[index]) & king)
			{
				res = MagicBishop(qb, occupancy) & (URDL[index]) & legalTargets;
				MoveInsert = extractWhiteMoves(res, baseMove, index, MoveInsert);
			}
			else
			{
				res = 0;
			}
			qb &= qb - 1;
		}
		qb = (Pieces[1] | Pieces[2]) & (~(PinnedPieces));
		while (qb)
		{
			u64 res = MagicBishop(qb, occupancy) & legalTargets;
			_BitScanForward64(&index, qb);
			qb &= qb - 1;
			MoveInsert = extractWhiteMoves(res, baseMove, index, MoveInsert);
		}
	};
	//Queen + Rook Move
	{
		u32 index;
		u64 qr = (Pieces[1] | Pieces[3]) & PinnedPieces;
		while (qr)
		{
			u64 res;
			_BitScanForward64(&index, qr);
			if ((UD[index]) & king)
			{
				res = MagicRook(qr, occupancy) & (UD[index]) & legalTargets;
				MoveInsert = extractWhiteMoves(res, baseMove, index, MoveInsert);
			}
			else if ((LR[index]) & king)
			{
				res = MagicRook(qr, occupancy) & (LR[index]) & legalTargets;
				MoveInsert = extractWhiteMoves(res, baseMove, index, MoveInsert);
			}
			else
			{
				res = 0;
			}
			qr &= qr - 1;
		}
		qr = (Pieces[1] | Pieces[3]) & (~(PinnedPieces));
		while (qr)
		{
			u64 res = MagicRook(qr, occupancy) & legalTargets;
			_BitScanForward64(&index, qr);
			qr &= qr - 1;
			MoveInsert = extractWhiteMoves(res, baseMove, index, MoveInsert);
		}
	};
	//Knight moves
	{
		u64 knights = Pieces[4] & (~(PinnedPieces));
		u32 index;
		while (knights)
		{
			_BitScanForward64(&index, knights);
			knights &= (knights - 1);
			MoveInsert = extractWhiteMoves(KnightSet[index] & legalTargets, baseMove, index, MoveInsert);
		}
	}
	if (MoveInsert == Start)
	{
		if (checks == 0)
		{
			*MoveInsert = 0;
		}
		else
		{
			*MoveInsert = 1;
		}
		MoveInsert++;
	}
	return MoveInsert;
}

u32* BitBoard::BlackLegalMoves(u32 *Start)
{
	u32 *MoveInsert = Start;
	u32 baseMove;
	{
		u32 index;
		if (EP)
		{
			_BitScanForward(&index, EP);
		}
		else
		{
			index = 8;
		}
		baseMove = index << 18 | rockad << 22 | silent << 12;
	}
	u32 kingIndex;
	u64 occupancy = Pieces[6] | Pieces[13], PinnedPieces = 0,
		legalTargets = 0, ownPieces = Pieces[13], king = Pieces[7],
		nOccupancy = ~occupancy;
	int checks = 0;
	if (king == 0)
	{
		*MoveInsert = 1;
		MoveInsert++;
		return MoveInsert;
	}
	//Find checks and pinned pieces
	{
		//pawns
		u64 EPiece = Pieces[4];
		u64 ThreatenedSquaresO = ((Pieces[5] << 7) & nColumns[7]);
		if (ThreatenedSquaresO & king)
		{
			checks++;
			legalTargets |= (ThreatenedSquaresO & king) >> 7;
		}
		ThreatenedSquaresO |= ((Pieces[5] << 9) & nColumns[0]);
		if (ThreatenedSquaresO & king && checks == 0)
		{
			checks++;
			legalTargets |= (ThreatenedSquaresO & king) >> 9;
		}
		u32 index;
		//Knights
		while (EPiece)
		{
			_BitScanForward64(&index, EPiece);
			EPiece &= EPiece - 1;
			ThreatenedSquaresO |= KnightSet[index];
			if (KnightSet[index] & king)
			{
				checks++;
				legalTargets |= one << index;
			}
		}
		//king
		_BitScanForward64(&index, Pieces[0]);
		ThreatenedSquaresO |= KingSet[index];
		//queen + rooks
		EPiece = Pieces[1] | Pieces[3];
		_BitScanForward64(&kingIndex, king);
		u64 kingAttacks = MagicRook(king, occupancy);
		u64 NKOccupancy = occupancy ^ king;
		u64 memory = EPiece & kingAttacks;
		EPiece &= ~(kingAttacks);
		if (memory)
		{
			u64 ret = MagicRook(memory, NKOccupancy), normal = MagicRook(memory, occupancy);
			ThreatenedSquaresO |= ret;
			legalTargets |= (kingAttacks & normal) | memory;
			checks++;
		}
		if (kingIndex > 63)
		{
			cout << king;
		}
		memory = EPiece & (UD[kingIndex] | LR[kingIndex]);
		EPiece &= ~(UD[kingIndex] | LR[kingIndex]);
		while (memory)
		{
			u64 normal = MagicRook(memory, NKOccupancy);
			ThreatenedSquaresO |= normal;
			PinnedPieces |= normal & kingAttacks;
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
		memory = EPiece & kingAttacks;
		EPiece &= ~(kingAttacks);
		if (memory)
		{
			u64 ret = MagicBishop(memory, NKOccupancy), normal = MagicBishop(memory, occupancy);
			ThreatenedSquaresO |= ret;
			legalTargets |= (kingAttacks & normal) | memory;
			checks++;
		}
		memory = EPiece & (ULDR[kingIndex] | URDL[kingIndex]);
		EPiece &= ~(ULDR[kingIndex] | URDL[kingIndex]);
		while (memory)
		{
			u64 normal = MagicBishop(memory, NKOccupancy);
			ThreatenedSquaresO |= normal;
			PinnedPieces |= normal & kingAttacks;
			memory &= memory - 1;
		}
		while (EPiece)
		{
			ThreatenedSquaresO |= MagicBishop(EPiece, NKOccupancy);;
			EPiece &= EPiece - 1;
		}
		u64 kingMoves = KingSet[kingIndex] & (~ThreatenedSquaresO) & (~ownPieces);
		MoveInsert = extractBlackMoves(kingMoves, baseMove, kingIndex, MoveInsert);
		if (checks > 1)
		{
			if (MoveInsert == Start)
			{
				*MoveInsert = 1;
				MoveInsert++;
			}
			return MoveInsert;
		}
		else if (checks == 0)
		{
			legalTargets = universal ^ ownPieces;
			if ((rockad & 2) && !((((u64)14) << 56) & ThreatenedSquaresO) && !((((u64)6) << 56) & occupancy))
			{
				u32 move = 59 | (57 << 6) | ((u32)7) << 29 | baseMove;
				*MoveInsert = move;
				MoveInsert++;
			}
			if ((rockad & 1) && !((((u64)56) << 56) & ThreatenedSquaresO) && !((((u64)112) << 56) & occupancy))
			{
				u32 move = 59 | (61 << 6) | ((u32)7) << 29 | baseMove;
				*MoveInsert = move;
				MoveInsert++;
			}
		}
	};
	//Pawn Move
	{
		u64 pawns = Pieces[12] & PinnedPieces, EPieces = Pieces[6];
		u32 index;
		while (pawns)
		{
			u64 res;
			_BitScanForward64(&index, pawns);
			if ((ULDR[index]) & king)
			{
				res = (((one << (index - 9)) & nColumns[7]) & EPieces);
			}
			else if ((URDL[index]) & king)
			{
				res = (((one << (index - 7)) & nColumns[0]) & EPieces);
			}
			else if ((UD[index]) & king)
			{
				res = (((one << (index - 8))) & nOccupancy);
				if (res & rows[5])
				{
					res |= ((res >> 8) & nOccupancy);
				}
			}
			else
			{
				res = 0;
			}
			res &= legalTargets;
			pawns &= pawns - 1;
			MoveInsert = extractBlackPawnMoves(res,baseMove, index, MoveInsert);
		}
		pawns = ((Pieces[12] & ~(PinnedPieces)) >> 9) & nColumns[7] & EPieces & legalTargets;
		while (pawns)
		{
			_BitScanForward64(&index, pawns);
			pawns &= pawns - 1;
			if (index > 7)
			{
				*MoveInsert = baseMove | index << 6 | (index + 9) | mailBox[index] << 29;
				MoveInsert++;
			}
			else
			{
				u32 move = baseMove | index << 6 | (index + 9) | mailBox[index] << 29;
				for (int i = 1; i < 5; i++)
				{
					*MoveInsert = move | (i << 26);
					MoveInsert++;
				}
			}
		}
		pawns = ((Pieces[12] & ~(PinnedPieces)) >> 7) & nColumns[0] & EPieces & legalTargets;
		while (pawns)
		{
			_BitScanForward64(&index, pawns);
			pawns &= pawns - 1;
			if (index > 7)
			{
				*MoveInsert = baseMove | index << 6 | (index + 7) | mailBox[index] << 29;
				MoveInsert++;
			}
			else
			{
				u32 move = baseMove | index << 6 | (index + 7) | mailBox[index] << 29;
				for (int i = 1; i < 5; i++)
				{
					*MoveInsert = move | (i << 26);
					MoveInsert++;
				}
			}
		}
		pawns = ((Pieces[12] & ~(PinnedPieces)) >> 8) & nOccupancy;
		u64 mem = ((pawns & rows[5]) >> 8) & nOccupancy & legalTargets;
		pawns &= legalTargets;
		while (pawns)
		{
			_BitScanForward64(&index, pawns);
			pawns &= pawns - 1;
			if (index > 7)
			{
				*MoveInsert = baseMove | index << 6 | (index + 8) | 7 << 29;
				MoveInsert++;
			}
			else
			{
				u32 move = baseMove | index << 6 | (index + 8) | 7 << 29;
				for (int i = 1; i < 5; i++)
				{
					*MoveInsert = move | (i << 26);
					MoveInsert++;
				}
			}
		}
		pawns = mem;
		while (pawns)
		{
			_BitScanForward64(&index, pawns);
			pawns &= pawns - 1;
			*MoveInsert = baseMove | index << 6 | (index + 16) | 7 << 29;
			MoveInsert++;
		}
		{
			u64 ep = ((u64)EP) << 24;
			if (ep & legalTargets)
			{
				ep >>= 8;
				pawns = ((Pieces[12] >> 9) & nColumns[7] & ep);
			}
			else
			{
				ep >>= 8;
				pawns = ((Pieces[12] >> 9) & nColumns[7] & ep & legalTargets);
			}
			if (pawns)
			{
				_BitScanForward64(&index, pawns);
				u64 mockOccupancy = (~((one << (index + 9)) | (one << (index + 8)))) & occupancy;
				mockOccupancy |= one << index;
				if (!((MagicRook(king, mockOccupancy) & (Pieces[1] | Pieces[3])) ||
					(MagicBishop(king, mockOccupancy) & (Pieces[1] | Pieces[2]))))
				{
					*MoveInsert = baseMove | (index + 9) | (index << 6);
					MoveInsert++;
				}
			}
			ep <<= 8;
			if (ep & legalTargets)
			{
				ep >>= 8;
				pawns = ((Pieces[12] >> 7) & nColumns[0] & ep);
			}
			else
			{
				ep >>= 8;
				pawns = ((Pieces[12] >> 7) & nColumns[0] & ep & legalTargets);
			}
			if (pawns)
			{
				_BitScanForward64(&index, pawns);
				u64 mockOccupancy = (~((one << (index + 7)) | (one << (index + 8)))) & occupancy;
				mockOccupancy |= one << index;
				if (!((MagicRook(king, mockOccupancy) & (Pieces[1] | Pieces[3])) ||
					(MagicBishop(king, mockOccupancy) & (Pieces[1] | Pieces[2]))))
				{
					*MoveInsert = baseMove | (index + 7) | (index << 6);
					MoveInsert++;
				}
			}
		}
	};
	//Queen + Bishop Move
	{
		u32 index;
		u64 qb = (Pieces[8] | Pieces[9]) & PinnedPieces;
		while (qb)
		{
			u64 res;
			_BitScanForward64(&index, qb);
			if ((ULDR[index]) & king)
			{
				res = MagicBishop(qb, occupancy) & ULDR[index] & legalTargets;
			}
			else if ((URDL[index]) & king)
			{
				res = MagicBishop(qb, occupancy) & (URDL[index]) & legalTargets;
			}
			else
			{
				res = 0;
			}
			qb &= qb - 1;
			MoveInsert = extractBlackMoves(res, baseMove, index, MoveInsert);
		}
		qb = (Pieces[8] | Pieces[9]) & (~(PinnedPieces));
		while (qb)
		{
			u64 res = MagicBishop(qb, occupancy) & legalTargets;
			_BitScanForward64(&index, qb);
			qb &= qb - 1;
			MoveInsert = extractBlackMoves(res, baseMove, index, MoveInsert);
		}
	}
	//Queen + Rook Move
	{
		u32 index;
		u64 qr = (Pieces[8] | Pieces[10]) & PinnedPieces;
		while (qr)
		{
			u64 res;
			_BitScanForward64(&index, qr);
			if ((UD[index]) & king)
			{
				res = MagicRook(qr, occupancy) & (UD[index]) & legalTargets;
			}
			else if ((LR[index]) & king)
			{
				res = MagicRook(qr, occupancy) & LR[index] & legalTargets;
			}
			else
			{
				res = 0;
			}
			qr &= qr - 1;
			MoveInsert = extractBlackMoves(res, baseMove, index, MoveInsert);
		}
		qr = (Pieces[8] | Pieces[10]) & (~(PinnedPieces));
		while (qr)
		{
			u64 res = MagicRook(qr, occupancy) & legalTargets;
			_BitScanForward64(&index, qr);
			qr &= qr - 1;
			MoveInsert = extractBlackMoves(res, baseMove, index, MoveInsert);
		}
	};
	//Knight moves
	{
		u64 knights = Pieces[11] & (~(PinnedPieces));
		u32 index;
		while (knights)
		{
			_BitScanForward64(&index, knights);
			knights &= (knights - 1);
			MoveInsert = extractBlackMoves(KnightSet[index] & legalTargets, baseMove, index, MoveInsert);
		}
	}
	if (MoveInsert == Start)
	{
		if (checks == 0)
		{
			*MoveInsert = 0;
			MoveInsert++;
		}
		else
		{
			*MoveInsert = 1;
			MoveInsert++;
		}
	}
	return MoveInsert;
}

int BitBoard::MakeMove(u32 move)
{
	u8 oldEP = EP;
	u8 from = (0b111111 & move), to = 0b111111 & (move >> 6);
	u8 moved = mailBox[from];
	if (moved == 5 || moved == 12 || mailBox[to] != 14)
	{
		silent = 0;
	}
	else
	{
		silent++;
	}
	RemovePiece(from);
	EP = 0;
	if (from == 0 || to == 0)
	{
		rockad &= (0b0111);
	}
	if (from == 7 || to == 7)
	{
		rockad &= (0b1011);
	}
	if (from == 63 || to == 63)
	{
		rockad &= (0b1110);
	}
	if (from == 56 || to == 56)
	{
		rockad &= (0b1101);
	}
	AddPiece(to, moved);
	u64 pos;
	switch (moved)
	{
	case 5://white pawn
		pos = one << to;
		if (move & (0b111 << 26))//upgrade
		{
			u8 promoteTo = (move >> 26) & 0b111;
			AddPiece(to, promoteTo);
		}
		else if ((pos >> 40) & oldEP)//EP
		{
			RemovePiece(to - 8);
		}
		else if (to - from > 9)
		{
			EP = (u8)one << (from & 0b111);
		}
		break;
	case 12://black pawn
		pos = one << to;
		if (move & (0b111 << 26))
		{
			u8 upgradeTo = ((move >> 26) & 0b111) + 7;
			AddPiece(to, upgradeTo);
		}
		else if ((pos >> 16) & oldEP)
		{
			RemovePiece(to + 8);
		}
		if (from - to > 9)
		{
			EP = (u8)one << (from & 0b111);
		}
		break;
	case 0://white king
		rockad &= 0b11;
		if (from - to == 2 || to - from == 2)
		{
			switch (to)
			{
			case 1:	//white kingside
				RemovePiece(0);
				AddPiece(2, 3);
				break;
			case 5:	//White Queen side
				RemovePiece(7);
				AddPiece(4, 3);
				break;
			default:
				break;
			}
		}
		break;
	case 7://black king
		rockad &= 0b1100;
		if (from - to == 2 || to - from == 2)
		{
			switch (to)
			{
			case 57://Black king sides
				RemovePiece(56);
				AddPiece(58, 10);
				break;
			case 61://Black Queen side
				RemovePiece(63);
				AddPiece(60, 10);
				break;
			default:
				break;
			}
		}
		break;
	default:
		break;
	}
	return 1;
}

//in order to store entire state 4 bit rockad, 3 bit EP, 6 bit silent = 13 bits; Would need to free another 5 bits;
//1 from taken, 1 from upgrade to, 1 rockad, 1 non silent, 1 upgrade
//Taken 3 = T ( << 29), upgradeTo [3] = U << 26, EP [1] = E << 25, Bit rockad [4] = R << 21, EP state [3] = s << 18, Silent [6] = S << 12, To [6] t << 6, From[6] == F << 6;
// TTTUUUERRRRsssSSSSSSttttttFFFFFF

void BitBoard::UnMakeMove(u32 move)
{
	silent = 0b111111 & (move >> 12);
	EP = (u8)one << (0b1111 & (move >> 18));
	rockad = (u8)0b1111 & (move >> 22);
	u8 from = (0b111111 & move), to = 0b111111 & (move >> 6);
	u8 moved = mailBox[to];
	u8 taken = move >> 29;
	if (moved < 6 || taken == 7)
	{
		taken += 7;
	}
	u64 pos;
	switch (moved)
	{
	case 5://white pawn
		pos = one << to;
		if ((pos >> 40) & EP)	//EP
		{
			AddPiece(from, 5);
			RemovePiece(to);
			AddPiece(to - 8, 12);
		}
		else
		{
			AddPiece(from, 5);
			AddPiece(to, taken);
		}
		break;
	case 12://black pawn
		pos = one << to;
		if ((pos >> 16) & EP)
		{
			AddPiece(from, 12);
			RemovePiece(to);
			AddPiece(to + 8, 5);
		}
		else
		{
			AddPiece(from, 12);
			AddPiece(to, taken);
		}
		break;
	default:
		//std
		if (move & (0b111 << 26))
		{
			u8 pawn;
			if (moved < 6)
			{
				pawn = 5;
			}
			else
			{
				pawn = 12;
			}
			AddPiece(from, pawn);
			AddPiece(to, taken);
		}
		else
		{
			AddPiece(from, moved);
			AddPiece(to, taken);
		}
		//rockad
		if (((to - from == 2) ||(from - to == 2)) && (moved == 0 ||moved == 7))
		{
			switch (to)
			{
			case 1:	//white kingside
				AddPiece(0, 3);
				RemovePiece(2);
				break;
			case 5:	//White Queen side
				AddPiece(7, 3);
				RemovePiece(4);
				break;
			case 57://Black king sides
				AddPiece(56, 10);
				RemovePiece(58);
				break;
			case 61://Black Queen side
				AddPiece(63, 10);
				RemovePiece(60);
				break;
			default:
				break;
			}
		}
		break;
	}
}
