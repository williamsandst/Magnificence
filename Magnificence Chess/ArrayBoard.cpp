#include "stdafx.h"
#include "Board.h"
#include "ArrayBoard.h"
#include "IO.h"
#include "Move.h"
#include <iostream>
#include <set>

//Data values for board array
//White:
//1 = pawn
//2 = rook
//3 = knight
//4 = bishop
//5 = queen
//6 = king
//Black
//11 = pawn
//12 = rook
//13 = knight
//14 = bishop
//15 = queen
//16 = king

const short diagonalArray[64] =
{ 8,9,10,11,12,13,14,15,
	7,8,9,10,11,12,13,14,
	6,7,8,9,10,11,12,13,
	5,6,7,8,9,10,11,12,
	4,5,6,7,8,9,10,11,
	3,4,5,6,7,8,9,10,
	2,3,4,5,6,7,8,9,
	1,2,3,4,5,6,7,8
};

const short antiDiagonalArray[64] =
{   15,14,13,12,11,10,9,8,
	14,13,12,11,10,9,8,7,
	13,12,11,10,9,8,7,6,
	12,11,10,9,8,7,6,5,
	11,10,9,8,7,6,5,4,
	10,9,8,7,6,5,4,3,
	9,8,7,6,5,4,3,2,
	8,7,6,5,4,3,2,1
};


void ArrayBoard::makeMove(__int16 move)
{
	//En passant
	if (enPassantSquare > -1)
	{
		if (enPassantSquare == Move::getTo(&move))
		{
			if (board[Move::getFrom(&move)] == 1) //White pawn
			{
				//If white pawn is performing en passant, delete pawn behind the square
				board[Move::getTo(&move)] = board[Move::getFrom(&move)];
				board[Move::getTo(&move) + 8] = 0;
				board[Move::getFrom(&move)] = 0;
				enPassantSquare = -1;
				return;
			}
			else if (board[Move::getFrom(&move)] == 11) //Black pawn
			{
				//If black pawn is performing en passant, delete pawn in front the square
				board[Move::getTo(&move)] = board[Move::getFrom(&move)];
				board[Move::getTo(&move) - 8] = 0;
				board[Move::getFrom(&move)] = 0;
				enPassantSquare = -1;
				return;
			}
		}
		enPassantSquare = -1;
	}

	//Castling and king moves
	if (castlingWhiteKingSide || castlingWhiteQueenSide)
	{
		if (castlingBlackKingSide || castlingBlackKingSide)
		{
			if (board[Move::getTo(&move)] == 2)
			{
				if (Move::getTo(&move) == 63)
					castlingWhiteKingSide = false;
				else if (Move::getTo(&move) == 56)
					castlingWhiteQueenSide = false;
			}
			else if (board[Move::getTo(&move)] == 12)
			{
				if (Move::getTo(&move) == 7)
					castlingBlackKingSide = false;
				else if (Move::getTo(&move) == 0)
					castlingBlackQueenSide = false;
			}
		}
		//King, castle or stop castling if applicable
		if (board[Move::getFrom(&move)] == 6) //Trying to move king
		{
			if (Move::getFrom(&move) == 60) //Is the king in starting postion?
			{
				if (Move::getTo(&move) == 62) //King side
				{
					//Perform castling
					board[60] = 0;
					board[61] = 2;
					board[62] = 6;
					board[63] = 0;
					castlingWhiteKingSide = false;
					castlingWhiteQueenSide = false;
					return;
				} //Trying to king-side castle
				else if (Move::getTo(&move) == 58) //Queen side
				{
					//Perform castling
					board[60] = 0;
					board[59] = 2;
					board[58] = 6;
					board[56] = 0;
					castlingWhiteKingSide = false;
					castlingWhiteQueenSide = false;
					return;
				} //Queenside castling
			}
			//If white king is moving, castling can no longer be performed
			castlingWhiteKingSide = false;
			castlingWhiteQueenSide = false;
			board[Move::getTo(&move)] = board[Move::getFrom(&move)];
			board[Move::getFrom(&move)] = 0;
			return;
		}
		//Rook moving, stop castling if applicable
		if (board[Move::getFrom(&move)] == 2) //White rook
		{
			if (Move::getFrom(&move) == 63) //White kingside rook is moving, disable kingside castling
			{
				castlingWhiteKingSide = false;
				board[Move::getTo(&move)] = board[Move::getFrom(&move)];
				board[Move::getFrom(&move)] = 0;
				return;
			}
			if (Move::getFrom(&move) == 56) //White queenside rook is moving, disable queenside castling
			{
				castlingWhiteQueenSide = false;
				board[Move::getTo(&move)] = board[Move::getFrom(&move)];
				board[Move::getFrom(&move)] = 0;
				return;
			}
		}
	}
	if (castlingBlackKingSide || castlingBlackQueenSide)
	{
		//King, castle or stop castling if applicable
		if (board[Move::getFrom(&move)] == 16) //Check if the black king is moving
		{
			if (Move::getFrom(&move) == 4) //Is black king in starting postion? Black castling check
			{
				if (Move::getTo(&move) == 6) //King side
				{
					//Perform castling
					board[4] = 0;
					board[5] = 12;
					board[6] = 16;
					board[7] = 0;
					castlingBlackKingSide = false;
					castlingBlackQueenSide = false;
					return;
				}
				else if (Move::getTo(&move) == 2) //Queen side
				{
					//Perform castling
					board[4] = 0;
					board[3] = 12;
					board[2] = 16;
					board[0] = 0;
					castlingBlackKingSide = false;
					castlingBlackQueenSide = false;
					return;
				}
			}
			//If black king is moving, castling can no longer be performed
			castlingBlackKingSide = false;
			castlingBlackQueenSide = false;
			board[Move::getTo(&move)] = board[Move::getFrom(&move)];
			board[Move::getFrom(&move)] = 0;
			return;
		}
		//Rook moving, stop castling if applicable
		if (board[Move::getFrom(&move)] == 12) //Black rook
		{
			if (Move::getFrom(&move) == 7) //Black kingside rook is moving, disable kingside castling
			{
				castlingBlackKingSide = false;
				board[Move::getTo(&move)] = board[Move::getFrom(&move)];
				board[Move::getFrom(&move)] = 0;
				return;
			}
			if (Move::getFrom(&move) == 0) //Black queenside rook is moving, disable queenside castling
			{
				castlingBlackQueenSide = false;
				board[Move::getTo(&move)] = board[Move::getFrom(&move)];
				board[Move::getFrom(&move)] = 0;
				return;
			}
		}
		//If rook that can castle is being taken, castling must be cancelled
	}

	//Specific piece rules
	//Moving rooks stops castling
	
	//Pawn (promotion and en passant square)
	if (board[Move::getFrom(&move)] == 1) //White pawn
	{
		if (Move::getTo(&move)+16 == Move::getFrom(&move)) //Double move, add en passant square
		{
			enPassantSquare = Move::getTo(&move) + 8;
			board[Move::getTo(&move)] = board[Move::getFrom(&move)];
			board[Move::getFrom(&move)] = 0;
			return;
		}
		else if (Move::getTo(&move) < 8) //Last line, promotion
		{
			board[Move::getFrom(&move)] = 0;
			board[Move::getTo(&move)] = Move::getPromotion(move) + 2; //Currently automaticly a queen, fix later
			return;
		}
	}
	else if (board[Move::getFrom(&move)] == 11) //Black pawn
	{
		if (Move::getTo(&move) == Move::getFrom(&move)+16) //Double move, add en passant square
		{
			enPassantSquare = Move::getFrom(&move) + 8;
			board[Move::getTo(&move)] = board[Move::getFrom(&move)];
			board[Move::getFrom(&move)] = 0;
			return;
		}
		if (Move::getTo(&move) > 55) //First line, promotion
		{
			board[Move::getFrom(&move)] = 0;
			board[Move::getTo(&move)] = Move::getPromotion(move) + 12;
			return;
		}
	}
	

	board[Move::getTo(&move)] = board[Move::getFrom(&move)];
	board[Move::getFrom(&move)] = 0;
	return;
}

void ArrayBoard::undoMove(__int16 move)
{

}

vector<__int16> ArrayBoard::generateWhiteLegalMoves()
{
	//Idea: use two vectors, one for attack and one for quiet moves for move ordering?
	vector<__int16> moves; //Average branching factor; too high? Does this even do anything?
	//Loop through every piece
	moves.reserve(32);
	//set<short> ignorePiecesSet;
	vector<short> ignorePiecesSet;
	int index;
	//Check for pinned pieces for king safety reasons
	//Check whether a black sliding piece can attack the black king
	//If they can, look if a piece is pinned.
	//	If a piece is pinned, generate moves only along that ray if possible
	//	Add to skipSet to remove from search

	//Todo:
	//Main functionality
		//More legality:
		//En passant special king check
		//Castling legality check, no rook check needed!
		//Evasion check: generate all moves that can be done under check

	//Optimisation: 0x88 board representation. Will take forever to change all indexes, but should
	//give big speedup
	//Reserve vector array size, experiment with best value

	int kingIndex;
	//Start by finding the king and various king information
	for (size_t i = 0; i < 64; i++)  //Find the king
	{
		if (board[i] == 6) //Found him!
		{
			kingIndex = i;
			break;
		}
	}

	int kingColumn = kingIndex % 8;
	int kingRow = kingIndex / 8;

	//Declare a variable to store a possible king attacker.
	//-1 means no attacker
	int kingAttackerIndex = -1;

	//Check if king is threatened by pawn or bishop
	//Pawn moves threatening king
	if (kingRow > 0 && board[kingIndex - 9] == 11 && diagonalArray[kingIndex] == diagonalArray[kingIndex - 9])
		kingAttackerIndex = kingIndex - 9;
	if (kingRow > 0 && board[kingIndex - 7] == 11 && antiDiagonalArray[kingIndex] == antiDiagonalArray[kingIndex - 7])
	{
		if (kingAttackerIndex < 0)
			kingAttackerIndex = kingIndex - 7; //Assign attacker
		else
			return generateWhiteEvasionMoves(moves, -1, kingIndex, ignorePiecesSet); //Two attackers
	}
	//Knights threatening king
	if (kingIndex + 17 < 64 && (board[kingIndex + 17] == 13) //17
		&& ((kingIndex + 17) / 8 == kingRow + 2))
	{
		if (kingAttackerIndex < 0)
			kingAttackerIndex = kingIndex + 17; //Assign attacker
		else
			return generateWhiteEvasionMoves(moves, -1, kingIndex, ignorePiecesSet); //Two attackers
	}
	if (kingIndex > 16 && (board[kingIndex - 17] == 13) //-17
		&& ((kingIndex - 17) / 8 == kingRow - 2))
	{
		if (kingAttackerIndex < 0)
			kingAttackerIndex = kingIndex - 17; //Assign attacker
		else
			return generateWhiteEvasionMoves(moves, -1, kingIndex, ignorePiecesSet); //Two attackers
	}
	if (kingIndex + 15 < 64 && (board[kingIndex + 15] == 13) //15
		&& ((kingIndex + 15) / 8 == kingRow + 2))
	{
		if (kingAttackerIndex < 0)
			kingAttackerIndex = kingIndex + 15; //Assign attacker
		else
			return generateWhiteEvasionMoves(moves, -1, kingIndex, ignorePiecesSet); //Two attackers
	}
	if (kingIndex > 14 && (board[kingIndex - 15] == 13) //-15
		&& ((kingIndex - 15) / 8 == kingRow - 2))
	{
		if (kingAttackerIndex < 0)
			kingAttackerIndex = kingIndex - 15; //Assign attacker
		else
			return generateWhiteEvasionMoves(moves, -1, kingIndex, ignorePiecesSet); //Two attackers
	}
	if (kingIndex + 10 < 64 && (board[kingIndex + 10] == 13) //10
		&& ((kingIndex + 10) / 8 == kingRow + 1))
	{
		if (kingAttackerIndex < 0)
			kingAttackerIndex = kingIndex + 10; //Assign attacker
		else
			return generateWhiteEvasionMoves(moves, -1, kingIndex, ignorePiecesSet); //Two attackers
	}
	if (kingIndex > 9 && (board[kingIndex - 10] == 13) //-10
		&& ((kingIndex - 10) / 8 == kingRow - 1))
	{
		if (kingAttackerIndex < 0)
			kingAttackerIndex = kingIndex - 10; //Assign attacker
		else
			return generateWhiteEvasionMoves(moves, -1, kingIndex, ignorePiecesSet); //Two attackers
	}
	if (kingIndex + 6 < 64 && (board[kingIndex + 6] == 13) //6
		&& ((kingIndex + 6) / 8 == kingRow + 1))
	{
		if (kingAttackerIndex < 0)
			kingAttackerIndex = kingIndex + 6; //Assign attacker
		else
			return generateWhiteEvasionMoves(moves, -1, kingIndex, ignorePiecesSet); //Two attackers
	}
	if (kingIndex > 5 && (board[kingIndex - 6] == 13) //-6
		&& ((kingIndex - 6) / 8 == kingRow - 1))
	{
		if (kingAttackerIndex < 0)
			kingAttackerIndex = kingIndex - 6; //Assign attacker
		else
			return generateWhiteEvasionMoves(moves, -1, kingIndex, ignorePiecesSet); //Two attackers
	}

	//A variable for whether there is a pinned piece at the momement
	bool noPinnedPiece = true;
	int pinIndex;

	//Here we look for pinned pieces and generates their limited moves
	//The piece is then added to the ignorePieceSet, which stops the normal generation
	//of moves for those pieces
	//This loop also looks for whether the king is in check
	//If the king is twice in check, it directly jumps to the check evasion move generator
	for (size_t i = 0; i < 64; i++) //Find black sliding pieces
	{
		//Check whether they align with the white king
		switch (board[i])
		{
		case 12: //Rook [FULLY FUNCTIONAL]
			if (i % 8 == kingColumn) //Same column
			{
				//King could be threatened by this piece. Do raycast
				//Check if king is above or below
				if (i > kingIndex) //Black rook moving UP
				{
					index = i - 8;
					//Check move until king is reached. There has to be a piece inbetween somewhere
					while (index != kingIndex)
					{
						if (board[index] == 0) //If it encounters nothing
						{
						}
						else if (board[index] > 8) //If it is a piece of its own color, break
						{
							noPinnedPiece = true;
							break;
						}
						else if (board[index] < 8) //White piece that is pinned! Jackpot
						{
							if (noPinnedPiece) //Found a pinned piece! Create moves here.
							{ 
								pinIndex = index;
								noPinnedPiece = false;
							}
							else //Discovered another white piece, no longer a pin. Quit yer search!
							{
								noPinnedPiece = true;
								break;
							}
						}
						index = index - 8;
					}
					if (!noPinnedPiece)
					{
						
						//100 % certain we found a pinned piece
						//Generate moves for either rook or queen moving DOWN
						ignorePiecesSet.push_back(pinIndex);
						if (board[pinIndex] == 2 || board[pinIndex] == 5)
						{
							index = kingIndex;
							while (index != i)
							{
								index = index + 8;
								if (index != pinIndex)
								{
									moves.push_back(Move::getInt16FromPos(pinIndex, index));
								}
							}
						}
						else if (board[pinIndex] == 1)
						{
							if (pinIndex < 56 && pinIndex > 47 && pinIndex - 16 > kingIndex) //Can doublemove and singlemove
							{
								moves.push_back(Move::getInt16FromPos(pinIndex, pinIndex - 16));
								moves.push_back(Move::getInt16FromPos(pinIndex, pinIndex - 8));
							}
							else if (pinIndex - 8 > kingIndex) //Can singlemove
								moves.push_back(Move::getInt16FromPos(pinIndex, pinIndex - 8));
						}
						noPinnedPiece = true;
						continue; //No need to check other direction, there's only 1 king
					}
					else if (index == kingIndex)
					{
						//No piece inbetween! The piece is in check!
						if (kingAttackerIndex < 0)
							kingAttackerIndex = i; //Assign attacker
						else
							return generateWhiteEvasionMoves(moves, -1, kingIndex, ignorePiecesSet); //Two attackers
					}
				}
				else //Other direction, black rook moving DOWN
				{
					index = i + 8;
					//Check move until king is reached. There has to be a piece inbetween somewhere
					while (index != kingIndex)
					{
						if (board[index] == 0) //If it encounters nothing
						{
						}
						else if (board[index] > 8) //If it is a piece of its own color, break
						{
							noPinnedPiece = true;
							break;
						}
						else if (board[index] < 8) //White piece that is pinned! Jackpot
						{
							if (noPinnedPiece) //Found a pinned piece! Create moves here.
							{
								pinIndex = index;
								noPinnedPiece = false;
							}
							else //Discovered another white piece, no longer a pin. Quit yer search!
							{
								noPinnedPiece = true;
								break;
							}
						}
						index = index + 8;
					}
					if (!noPinnedPiece)
					{

						//100 % certain we found a pinned piece
						//Generate moves for either rook or queen moving UP
						ignorePiecesSet.push_back(pinIndex);
						if (board[pinIndex] == 2 || board[pinIndex] == 5)
						{
							index = kingIndex;
							while (index != i)
							{
								index = index - 8;
								if (index != pinIndex)
								{
									moves.push_back(Move::getInt16FromPos(pinIndex, index));
								}
							}
						}
						else if (board[pinIndex] == 1)
						{
							if (pinIndex < 56 && pinIndex > 47 && pinIndex - 16 > i) //Can doublemove and singlemove
							{ 
								moves.push_back(Move::getInt16FromPos(pinIndex, pinIndex - 16));
								moves.push_back(Move::getInt16FromPos(pinIndex, pinIndex - 8));
							}
							else if (pinIndex - 8 > i) //Can singlemove
								moves.push_back(Move::getInt16FromPos(pinIndex, pinIndex - 8));
						}
						noPinnedPiece = true;
						continue; //No need to check other direction, there's only 1 king
					}
					else if (index == kingIndex)
					{
						//No piece inbetween! The piece is in check!
						if (kingAttackerIndex < 0)
							kingAttackerIndex = i; //Assign attacker
						else
							return generateWhiteEvasionMoves(moves, -1, kingIndex, ignorePiecesSet); //Two attackers
					}
				}

			}
			else if (i / 8 == kingRow) //Same row
			{
				//King could be threatened by this piece. Do raycast
				//Check if king is above or below
				if (i > kingIndex) //Black rook moving LEFT
				{
					index = i - 1;
					//Check move until king is reached. There has to be a piece inbetween somewhere
					while (index != kingIndex)
					{
						if (board[index] == 0) //If it encounters nothing
						{
						}
						else if (board[index] > 8) //If it is a piece of its own color, break
						{
							noPinnedPiece = true;
							break;
						}
						else if (board[index] < 8) //White piece that is pinned! Jackpot
						{
							if (noPinnedPiece) //Found a pinned piece! Create moves here.
							{
								pinIndex = index;
								noPinnedPiece = false;
							}
							else //Discovered another white piece, no longer a pin. Quit yer search!
							{
								noPinnedPiece = true;
								break;
							}
						}
						index = index - 1;
					}
					if (!noPinnedPiece)
					{

						//100 % certain we found a pinned piece
						//Generate moves for either rook or queen moving RIGHT
						ignorePiecesSet.push_back(pinIndex);
						if (board[pinIndex] == 2 || board[pinIndex] == 5)
						{
							index = kingIndex;
							while (index != i)
							{
								index = index + 1;
								if (index != pinIndex)
								{
									moves.push_back(Move::getInt16FromPos(pinIndex, index));
								}
							}
						}
						noPinnedPiece = true;
						continue; //No need to check other direction, there's only 1 king
					}
					else if (index == kingIndex)
					{
						//No piece inbetween! The piece is in check!
						if (kingAttackerIndex < 0)
							kingAttackerIndex = i; //Assign attacker
						else
							return generateWhiteEvasionMoves(moves, -1, kingIndex, ignorePiecesSet); //Two attackers
					}
				}
				else //Other direction, black rook moving RIGHT
				{
					index = i + 1;
					//Check move until king is reached. There has to be a piece inbetween somewhere
					while (index != kingIndex)
					{
						if (board[index] == 0) //If it encounters nothing
						{
						}
						else if (board[index] > 8) //If it is a piece of its own color, break
						{
							noPinnedPiece = true;
							break;
						}
						else if (board[index] < 8) //White piece that is pinned! Jackpot
						{
							if (noPinnedPiece) //Found a pinned piece! Create moves here.
							{
								pinIndex = index;
								noPinnedPiece = false;
							}
							else //Discovered another white piece, no longer a pin. Quit yer search!
							{
								noPinnedPiece = true;
								break;
							}
						}
						index = index + 1;
					}
					if (!noPinnedPiece)
					{

						//100 % certain we found a pinned piece
						//Generate moves for either rook or queen moving UP
						ignorePiecesSet.push_back(pinIndex);
						if (board[pinIndex] == 2 || board[pinIndex] == 5)
						{
							index = kingIndex;
							while (index != i)
							{
								index = index - 1;
								if (index != pinIndex)
								{
									moves.push_back(Move::getInt16FromPos(pinIndex, index));
								}
							}
						}
						noPinnedPiece = true;
						continue; //No need to check other direction, there's only 1 king
					}
					else if (index == kingIndex)
					{
						//No piece inbetween! The piece is in check!
						if (kingAttackerIndex < 0)
							kingAttackerIndex = i; //Assign attacker
						else
							return generateWhiteEvasionMoves(moves, -1, kingIndex, ignorePiecesSet); //Two attackers
					}
				}
			}
			break;
		case 14: //Bishop [FULLY FUNCTIONAL]
			if (diagonalArray[i] == diagonalArray[kingIndex]) //Diagonal: main file a1h8
			{
				if (i > kingIndex) //Black bishop moving diagonal UP
				{
					index = i - 9;
					//Check move until king is reached. There has to be a piece inbetween somewhere
					while (index != kingIndex)
					{
						if (board[index] == 0) //If it encounters nothing
						{
						}
						else if (board[index] > 8) //If it is a piece of its own color, break
						{
							noPinnedPiece = true;
							break;
						}
						else if (board[index] < 8) //White piece that is pinned! Jackpot
						{
							if (noPinnedPiece) //Found a pinned piece! Create moves here.
							{
								pinIndex = index;
								noPinnedPiece = false;
							}
							else //Discovered another white piece, no longer a pin. Quit yer search!
							{
								noPinnedPiece = true;
								break;
							}
						}
						index = index - 9;
					}
					if (!noPinnedPiece)
					{

						//100 % certain we found a pinned piece
						//Generate moves for either rook or queen moving DOWN
						ignorePiecesSet.push_back(pinIndex);
						if (board[pinIndex] == 4 || board[pinIndex] == 5)
						{
							index = kingIndex;
							while (index != i)
							{
								index = index + 9;
								if (index != pinIndex)
								{
									moves.push_back(Move::getInt16FromPos(pinIndex, index));
								}
							}
						}
						noPinnedPiece = true;
						continue; //No need to check other direction, there's only 1 king
					}
					else if (index == kingIndex)
					{
						//No piece inbetween! The piece is in check!
						if (kingAttackerIndex < 0)
							kingAttackerIndex = i; //Assign attacker
						else
							return generateWhiteEvasionMoves(moves, -1, kingIndex, ignorePiecesSet); //Two attackers
					}
				}
				else //Other direction, black rook moving DOWN
				{
					index = i + 9;
					//Check move until king is reached. There has to be a piece inbetween somewhere
					while (index != kingIndex)
					{
						if (board[index] == 0) //If it encounters nothing
						{
						}
						else if (board[index] > 8) //If it is a piece of its own color, break
						{
							noPinnedPiece = true;
							break;
						}
						else if (board[index] < 8) //White piece that is pinned! Jackpot
						{
							if (noPinnedPiece) //Found a pinned piece! Create moves here.
							{
								pinIndex = index;
								noPinnedPiece = false;
							}
							else //Discovered another white piece, no longer a pin. Quit yer search!
							{
								noPinnedPiece = true;
								break;
							}
						}
						index = index + 9;
					}
					if (!noPinnedPiece)
					{

						//100 % certain we found a pinned piece
						//Generate moves for either rook or queen moving UP
						ignorePiecesSet.push_back(pinIndex);
						if (board[pinIndex] == 4 || board[pinIndex] == 5)
						{
							index = kingIndex;
							while (index != i)
							{
								index = index - 9;
								if (index != pinIndex)
								{
									moves.push_back(Move::getInt16FromPos(pinIndex, index));
								}
							}
						}
						else if (board[pinIndex] == 1) //Pawn, check if it can take directly
						{
							if (i == pinIndex - 9)
							{
								moves.push_back(Move::getInt16FromPos(pinIndex, i));
							}
						}
						noPinnedPiece = true;
						continue; //No need to check other direction, there's only 1 king
					}
					else if (index == kingIndex)
					{
						//No piece inbetween! The piece is in check!
						if (kingAttackerIndex < 0)
							kingAttackerIndex = i; //Assign attacker
						else
							return generateWhiteEvasionMoves(moves, -1, kingIndex, ignorePiecesSet); //Two attackers
					}
				}

			}
			else if (antiDiagonalArray[i] == antiDiagonalArray[kingIndex])
			{
				if (i > kingIndex) //Black bishop moving diagonal UP
				{
					index = i - 7;
					//Check move until king is reached. There has to be a piece inbetween somewhere
					while (index != kingIndex)
					{
						if (board[index] == 0) //If it encounters nothing
						{
						}
						else if (board[index] > 8) //If it is a piece of its own color, break
						{
							noPinnedPiece = true;
							break;
						}
						else if (board[index] < 8) //White piece that is pinned! Jackpot
						{
							if (noPinnedPiece) //Found a pinned piece! Create moves here.
							{
								pinIndex = index;
								noPinnedPiece = false;
							}
							else //Discovered another white piece, no longer a pin. Quit yer search!
							{
								noPinnedPiece = true;
								break;
							}
						}
						index = index - 7;
					}
					if (!noPinnedPiece)
					{

						//100 % certain we found a pinned piece
						//Generate moves for either rook or queen moving DOWN
						ignorePiecesSet.push_back(pinIndex);
						if (board[pinIndex] == 4 || board[pinIndex] == 5)
						{
							index = kingIndex;
							while (index != i)
							{
								index = index + 7;
								if (index != pinIndex)
								{
									moves.push_back(Move::getInt16FromPos(pinIndex, index));
								}
							}
						}
						noPinnedPiece = true;
						continue; //No need to check other direction, there's only 1 king
					}
					else if (index == kingIndex)
					{
						//No piece inbetween! The piece is in check!
						if (kingAttackerIndex < 0)
							kingAttackerIndex = i; //Assign attacker
						else
							return generateWhiteEvasionMoves(moves, -1, kingIndex, ignorePiecesSet); //Two attackers
					}
				}
				else //Other direction, black rook moving DOWN
				{
					index = i + 7;
					//Check move until king is reached. There has to be a piece inbetween somewhere
					while (index != kingIndex)
					{
						if (board[index] == 0) //If it encounters nothing
						{
						}
						else if (board[index] > 8) //If it is a piece of its own color, break
						{
							noPinnedPiece = true;
							break;
						}
						else if (board[index] < 8) //White piece that is pinned! Jackpot
						{
							if (noPinnedPiece) //Found a pinned piece! Create moves here.
							{
								pinIndex = index;
								noPinnedPiece = false;
							}
							else //Discovered another white piece, no longer a pin. Quit yer search!
							{
								noPinnedPiece = true;
								break;
							}
						}
						index = index + 7;
					}
					if (!noPinnedPiece)
					{

						//100 % certain we found a pinned piece
						//Generate moves for either rook or queen moving UP
						ignorePiecesSet.push_back(pinIndex);
						if (board[pinIndex] == 4 || board[pinIndex] == 5)
						{
							index = kingIndex;
							while (index != i)
							{
								index = index - 7;
								if (index != pinIndex)
								{
									moves.push_back(Move::getInt16FromPos(pinIndex, index));
								}
							}
						}
						else if (board[pinIndex] == 1) //Pawn, check if it can take directly
						{
							if (i == pinIndex - 7)
							{
								moves.push_back(Move::getInt16FromPos(pinIndex, i));
							}
						}
						noPinnedPiece = true;
						continue; //No need to check other direction, there's only 1 king
					}
					else if (index == kingIndex)
					{
						//No piece inbetween! The piece is in check!
						if (kingAttackerIndex < 0)
							kingAttackerIndex = i; //Assign attacker
						else
							return generateWhiteEvasionMoves(moves, -1, kingIndex, ignorePiecesSet); //Two attackers
					}
				}

			}
			break;
		case 15: //Queen [FULLY FUNCTIONAL]
			if (i % 8 == kingColumn) //Same column
			{
				//King could be threatened by this piece. Do raycast
				//Check if king is above or below
				if (i > kingIndex) //Black queen moving UP
				{
					index = i - 8;
					//Check move until king is reached. There has to be a piece inbetween somewhere
					while (index != kingIndex)
					{
						if (board[index] == 0) //If it encounters nothing
						{
						}
						else if (board[index] > 8) //If it is a piece of its own color, break
						{
							noPinnedPiece = true;
							break;
						}
						else if (board[index] < 8) //White piece that is pinned! Jackpot
						{
							if (noPinnedPiece) //Found a pinned piece! Create moves here.
							{
								pinIndex = index;
								noPinnedPiece = false;
							}
							else //Discovered another white piece, no longer a pin. Quit yer search!
							{
								noPinnedPiece = true;
								break;
							}
						}
						index = index - 8;
					}
					if (!noPinnedPiece)
					{

						//100 % certain we found a pinned piece
						//Generate moves for either rook or queen moving DOWN
						ignorePiecesSet.push_back(pinIndex);
						if (board[pinIndex] == 2 || board[pinIndex] == 5)
						{
							index = kingIndex;
							while (index != i)
							{
								index = index + 8;
								if (index != pinIndex)
								{
									moves.push_back(Move::getInt16FromPos(pinIndex, index));
								}
							}
						}
						else if (board[pinIndex] == 1)
						{
							if (pinIndex < 56 && pinIndex > 47 && pinIndex - 16 > kingIndex) //Can doublemove and singlemove
							{
								moves.push_back(Move::getInt16FromPos(pinIndex, pinIndex - 16));
								moves.push_back(Move::getInt16FromPos(pinIndex, pinIndex - 8));
							}
							else if (pinIndex - 8 > kingIndex) //Can singlemove
								moves.push_back(Move::getInt16FromPos(pinIndex, pinIndex - 8));
						}
						noPinnedPiece = true;
						continue; //No need to check other direction, there's only 1 king
					}
					else if (index == kingIndex)
					{
						//No piece inbetween! The piece is in check!
						if (kingAttackerIndex < 0)
							kingAttackerIndex = i; //Assign attacker
						else
							return generateWhiteEvasionMoves(moves, -1, kingIndex, ignorePiecesSet); //Two attackers
					}

				}
				else //Other direction, black queen moving DOWN
				{
					index = i + 8;
					//Check move until king is reached. There has to be a piece inbetween somewhere
					while (index != kingIndex)
					{
						if (board[index] == 0) //If it encounters nothing
						{
						}
						else if (board[index] > 8) //If it is a piece of its own color, break
						{
							noPinnedPiece = true;
							break;
						}
						else if (board[index] < 8) //White piece that is pinned! Jackpot
						{
							if (noPinnedPiece) //Found a pinned piece! Create moves here.
							{
								pinIndex = index;
								noPinnedPiece = false;
							}
							else //Discovered another white piece, no longer a pin. Quit yer search!
							{
								noPinnedPiece = true;
								break;
							}
						}
						index = index + 8;
					}
					if (!noPinnedPiece)
					{

						//100 % certain we found a pinned piece
						//Generate moves for either rook or queen moving UP
						ignorePiecesSet.push_back(pinIndex);
						if (board[pinIndex] == 2 || board[pinIndex] == 5)
						{
							index = kingIndex;
							while (index != i)
							{
								index = index - 8;
								if (index != pinIndex)
								{
									moves.push_back(Move::getInt16FromPos(pinIndex, index));
								}
							}
						}
						else if (board[pinIndex] == 1)
						{
							if (pinIndex < 56 && pinIndex > 47 && pinIndex - 16 > i) //Can doublemove and singlemove
							{
								moves.push_back(Move::getInt16FromPos(pinIndex, pinIndex - 16));
								moves.push_back(Move::getInt16FromPos(pinIndex, pinIndex - 8));
							}
							else if (pinIndex - 8 > i) //Can singlemove
								moves.push_back(Move::getInt16FromPos(pinIndex, pinIndex - 8));
						}
						noPinnedPiece = true;
						continue; //No need to check other direction, there's only 1 king
					}
					else if (index == kingIndex)
					{
						//No piece inbetween! The piece is in check!
						if (kingAttackerIndex < 0)
							kingAttackerIndex = i; //Assign attacker
						else
							return generateWhiteEvasionMoves(moves, -1, kingIndex, ignorePiecesSet); //Two attackers
					}
				}

			}
			else if (i / 8 == kingRow) //Same row
			{
				//King could be threatened by this piece. Do raycast
				//Check if king is above or below
				if (i > kingIndex) //Black queen moving LEFT
				{
					index = i - 1;
					//Check move until king is reached. There has to be a piece inbetween somewhere
					while (index != kingIndex)
					{
						if (board[index] == 0) //If it encounters nothing
						{
						}
						else if (board[index] > 8) //If it is a piece of its own color, break
						{
							noPinnedPiece = true;
							break;
						}
						else if (board[index] < 8) //White piece that is pinned! Jackpot
						{
							if (noPinnedPiece) //Found a pinned piece! Create moves here.
							{
								pinIndex = index;
								noPinnedPiece = false;
							}
							else //Discovered another white piece, no longer a pin. Quit yer search!
							{
								noPinnedPiece = true;
								break;
							}
						}
						index = index - 1;
					}
					if (!noPinnedPiece)
					{

						//100 % certain we found a pinned piece
						//Generate moves for either rook or queen moving RIGHT
						ignorePiecesSet.push_back(pinIndex);
						if (board[pinIndex] == 2 || board[pinIndex] == 5)
						{
							index = kingIndex;
							while (index != i)
							{
								index = index + 1;
								if (index != pinIndex)
								{
									moves.push_back(Move::getInt16FromPos(pinIndex, index));
								}
							}
						}
						noPinnedPiece = true;
						continue; //No need to check other direction, there's only 1 king
					}
					else if (index == kingIndex)
					{
						//No piece inbetween! The piece is in check!
						if (kingAttackerIndex < 0)
							kingAttackerIndex = i; //Assign attacker
						else
							return generateWhiteEvasionMoves(moves, -1, kingIndex, ignorePiecesSet); //Two attackers
					}
				}
				else //Other direction, black queen moving RIGHT
				{
					index = i + 1;
					//Check move until king is reached. There has to be a piece inbetween somewhere
					while (index != kingIndex)
					{
						if (board[index] == 0) //If it encounters nothing
						{
						}
						else if (board[index] > 8) //If it is a piece of its own color, break
						{
							noPinnedPiece = true;
							break;
						}
						else if (board[index] < 8) //White piece that is pinned! Jackpot
						{
							if (noPinnedPiece) //Found a pinned piece! Create moves here.
							{
								pinIndex = index;
								noPinnedPiece = false;
							}
							else //Discovered another white piece, no longer a pin. Quit yer search!
							{
								noPinnedPiece = true;
								break;
							}
						}
						index = index + 1;
					}
					if (!noPinnedPiece)
					{

						//100 % certain we found a pinned piece
						//Generate moves for either rook or queen moving UP
						ignorePiecesSet.push_back(pinIndex);
						if (board[pinIndex] == 2 || board[pinIndex] == 5)
						{
							index = kingIndex;
							while (index != i)
							{
								index = index - 1;
								if (index != pinIndex)
								{
									moves.push_back(Move::getInt16FromPos(pinIndex, index));
								}
							}
						}
						noPinnedPiece = true;
						continue; //No need to check other direction, there's only 1 king
					}
					else if (index == kingIndex)
					{
						//No piece inbetween! The piece is in check!
						if (kingAttackerIndex < 0)
							kingAttackerIndex = i; //Assign attacker
						else
							return generateWhiteEvasionMoves(moves, -1, kingIndex, ignorePiecesSet); //Two attackers
					}
				}
			}
			else if (diagonalArray[i] == diagonalArray[kingIndex]) //Diagonal: main file a1h8
			{
				if (i > kingIndex) //Black bishop moving diagonal UP
				{
					index = i - 9;
					//Check move until king is reached. There has to be a piece inbetween somewhere
					while (index != kingIndex)
					{
						if (board[index] == 0) //If it encounters nothing
						{
						}
						else if (board[index] > 8) //If it is a piece of its own color, break
						{
							noPinnedPiece = true;
							break;
						}
						else if (board[index] < 8) //White piece that is pinned! Jackpot
						{
							if (noPinnedPiece) //Found a pinned piece! Create moves here.
							{
								pinIndex = index;
								noPinnedPiece = false;
							}
							else //Discovered another white piece, no longer a pin. Quit yer search!
							{
								noPinnedPiece = true;
								break;
							}
						}
						index = index - 9;
					}
					if (!noPinnedPiece)
					{

						//100 % certain we found a pinned piece
						//Generate moves for either rook or queen moving DOWN
						ignorePiecesSet.push_back(pinIndex);
						if (board[pinIndex] == 4 || board[pinIndex] == 5)
						{
							index = kingIndex;
							while (index != i)
							{
								index = index + 9;
								if (index != pinIndex)
								{
									moves.push_back(Move::getInt16FromPos(pinIndex, index));
								}
							}
						}
						noPinnedPiece = true;
						continue; //No need to check other direction, there's only 1 king
					}
					else if (index == kingIndex)
					{
						//No piece inbetween! The piece is in check!
						if (kingAttackerIndex < 0)
							kingAttackerIndex = i; //Assign attacker
						else
							return generateWhiteEvasionMoves(moves, -1, kingIndex, ignorePiecesSet); //Two attackers
					}
				}
				else //Other direction, black rook moving DOWN
				{
					index = i + 9;
					//Check move until king is reached. There has to be a piece inbetween somewhere
					while (index != kingIndex)
					{
						if (board[index] == 0) //If it encounters nothing
						{
						}
						else if (board[index] > 8) //If it is a piece of its own color, break
						{
							noPinnedPiece = true;
							break;
						}
						else if (board[index] < 8) //White piece that is pinned! Jackpot
						{
							if (noPinnedPiece) //Found a pinned piece! Create moves here.
							{
								pinIndex = index;
								noPinnedPiece = false;
							}
							else //Discovered another white piece, no longer a pin. Quit yer search!
							{
								noPinnedPiece = true;
								break;
							}
						}
						index = index + 9;
					}
					if (!noPinnedPiece)
					{

						//100 % certain we found a pinned piece
						//Generate moves for either rook or queen moving UP
						ignorePiecesSet.push_back(pinIndex);
						if (board[pinIndex] == 4 || board[pinIndex] == 5)
						{
							index = kingIndex;
							while (index != i)
							{
								index = index - 9;
								if (index != pinIndex)
								{
									moves.push_back(Move::getInt16FromPos(pinIndex, index));
								}
							}
						}
						else if (board[pinIndex] == 1) //Pawn, check if it can take directly
						{
							if (i == pinIndex - 9)
							{
								moves.push_back(Move::getInt16FromPos(pinIndex, i));
							}
						}
						noPinnedPiece = true;
						continue; //No need to check other direction, there's only 1 king
					}
					else if (index == kingIndex)
					{
						//No piece inbetween! The piece is in check!
						if (kingAttackerIndex < 0)
							kingAttackerIndex = i; //Assign attacker
						else
							return generateWhiteEvasionMoves(moves, -1, kingIndex, ignorePiecesSet); //Two attackers
					}
				}

			}
			else if (antiDiagonalArray[i] == antiDiagonalArray[kingIndex])
			{
				if (i > kingIndex) //Black bishop moving diagonal UP
				{
					index = i - 7;
					//Check move until king is reached. There has to be a piece inbetween somewhere
					while (index != kingIndex)
					{
						if (board[index] == 0) //If it encounters nothing
						{
						}
						else if (board[index] > 8) //If it is a piece of its own color, break
						{
							noPinnedPiece = true;
							break;
						}
						else if (board[index] < 8) //White piece that is pinned! Jackpot
						{
							if (noPinnedPiece) //Found a pinned piece! Create moves here.
							{
								pinIndex = index;
								noPinnedPiece = false;
							}
							else //Discovered another white piece, no longer a pin. Quit yer search!
							{
								noPinnedPiece = true;
								break;
							}
						}
						index = index - 7;
					}
					if (!noPinnedPiece)
					{

						//100 % certain we found a pinned piece
						//Generate moves for either rook or queen moving DOWN
						ignorePiecesSet.push_back(pinIndex);
						if (board[pinIndex] == 4 || board[pinIndex] == 5)
						{
							index = kingIndex;
							while (index != i)
							{
								index = index + 7;
								if (index != pinIndex)
								{
									moves.push_back(Move::getInt16FromPos(pinIndex, index));
								}
							}
						}
						noPinnedPiece = true;
						continue; //No need to check other direction, there's only 1 king
					}
					else if (index == kingIndex)
					{
						//No piece inbetween! The piece is in check!
						if (kingAttackerIndex < 0)
							kingAttackerIndex = i; //Assign attacker
						else
							return generateWhiteEvasionMoves(moves, -1, kingIndex, ignorePiecesSet); //Two attackers
					}
				}
				else //Other direction, black rook moving DOWN
				{
					index = i + 7;
					//Check move until king is reached. There has to be a piece inbetween somewhere
					while (index != kingIndex)
					{
						if (board[index] == 0) //If it encounters nothing
						{
						}
						else if (board[index] > 8) //If it is a piece of its own color, break
						{
							noPinnedPiece = true;
							break;
						}
						else if (board[index] < 8) //White piece that is pinned! Jackpot
						{
							if (noPinnedPiece) //Found a pinned piece! Create moves here.
							{
								pinIndex = index;
								noPinnedPiece = false;
							}
							else //Discovered another white piece, no longer a pin. Quit yer search!
							{
								noPinnedPiece = true;
								break;
							}
						}
						index = index + 7;
					}
					if (!noPinnedPiece)
					{

						//100 % certain we found a pinned piece
						//Generate moves for either rook or queen moving UP
						ignorePiecesSet.push_back(pinIndex);
						if (board[pinIndex] == 4 || board[pinIndex] == 5)
						{
							index = kingIndex;
							while (index != i)
							{
								index = index - 7;
								if (index != pinIndex)
								{
									moves.push_back(Move::getInt16FromPos(pinIndex, index));
								}
							}
						}
						else if (board[pinIndex] == 1) //Pawn, check if it can take directly
						{
							if (i == pinIndex - 7)
							{
								moves.push_back(Move::getInt16FromPos(pinIndex, i));
							}
						}
						noPinnedPiece = true;
						continue; //No need to check other direction, there's only 1 king
					}
					else if (index == kingIndex)
					{
						//No piece inbetween! The piece is in check!
						if (kingAttackerIndex < 0)
							kingAttackerIndex = i; //Assign attacker
						else
							return generateWhiteEvasionMoves(moves, -1, kingIndex, ignorePiecesSet); //Two attackers
					}
				}

			}
			break;
		}
	}

	//If king is in check by one attacker
	if (!(kingAttackerIndex < 0))
	{
		//Generate white evasion moves
		return generateWhiteEvasionMoves(moves, kingAttackerIndex, kingIndex, ignorePiecesSet);
	}

	//Generates all the normal moves
	for (size_t i = 0; i < 64; i++)
	{
		if (find(ignorePiecesSet.begin(), ignorePiecesSet.end(), i) != ignorePiecesSet.end())
			continue;
		switch (board[i]) //Switch to make allow for compiler optimisation
		{
		case 0: //Do nothing
			break;
		case 1: //White pawn
			if (board[i - 8] == 0) //Move forward
			{
				if (i < 16) //Promotion straight
				{
					//Only queen is necessary here later on!
					moves.push_back(Move::getInt16FromPos(i, i - 8, 0, 0));
					moves.push_back(Move::getInt16FromPos(i, i - 8, 1, 0));
					moves.push_back(Move::getInt16FromPos(i, i - 8, 2, 0));
					moves.push_back(Move::getInt16FromPos(i, i - 8, 3, 0));
				}
				else
				{
					moves.push_back(Move::getInt16FromPos(i, i - 8));
					if (i > 47 && i < 56 && board[i - 16] == 0)
					{
						moves.push_back(Move::getInt16FromPos(i, i - 16));
					}
				}
			}
			//Taking pieces
			if (((i - 7) / 8 == (i / 8)-1) && board[i - 7] > 8)
			{
				if (i < 16) //Promotion straight
				{
					//Only queen is necessary here later on!
					moves.push_back(Move::getInt16FromPos(i, i - 7, 0, 0));
					moves.push_back(Move::getInt16FromPos(i, i - 7, 1, 0));
					moves.push_back(Move::getInt16FromPos(i, i - 7, 2, 0));
					moves.push_back(Move::getInt16FromPos(i, i - 7, 3, 0));
				}
				else
					moves.push_back(Move::getInt16FromPos(i, i - 7));
			}
			if (((i - 9) / 8 == (i / 8)-1) && board[i - 9] > 8)
			{
				if (i < 16) //Promotion straight
				{
					//Only queen is necessary here later on!
					moves.push_back(Move::getInt16FromPos(i, i - 9, 0, 0));
					moves.push_back(Move::getInt16FromPos(i, i - 9, 1, 0));
					moves.push_back(Move::getInt16FromPos(i, i - 9, 2, 0));
					moves.push_back(Move::getInt16FromPos(i, i - 9, 3, 0));
				}
				else
					moves.push_back(Move::getInt16FromPos(i, i - 9));
			}
			break;
		case 2: //White rook
			//Iterate these: +8, +1, -1, -8
			index = i + 8;
			while (index < 64) //+8, outside board check
			{
				if (board[index] != 0)
				{
					if (board[index] > 8) //Can take
					{
						moves.push_back(Move::getInt16FromPos(i, index));
					}
					break; //Otherwise, stop looking
				}
				//Empty square, add move
				moves.push_back(Move::getInt16FromPos(i, index));
				index = index + 8;
			}
			index = i - 8;
			while (index > -1) //-8, outside board check
			{
				if (board[index] != 0)
				{
					if (board[index] > 8) //Can take
					{
						moves.push_back(Move::getInt16FromPos(i, index));
					}
					break; //Otherwise, stop looking
				}
				//Empty square, add move
				moves.push_back(Move::getInt16FromPos(i, index));
				index = index - 8;
			}
			index = i + 1;
			while (index / 8 == i / 8) //+8, outside board check
			{
				if (board[index] != 0)
				{
					if (board[index] > 8) //Can take
					{
						moves.push_back(Move::getInt16FromPos(i, index));
					}
					break; //Otherwise, stop looking
				}
				//Empty square, add move
				moves.push_back(Move::getInt16FromPos(i, index));
				index = index + 1;
			}
			index = i - 1;
			while (index > -1 && index / 8 == i / 8) //+8, outside board check
			{
				if (board[index] != 0)
				{
					if (board[index] > 8) //Can take
					{
						moves.push_back(Move::getInt16FromPos(i, index));
					}
					break; //Otherwise, stop looking
				}
				//Empty square, add move
				moves.push_back(Move::getInt16FromPos(i, index));
				index = index - 1;
			}
			break;
		case 3: //White knight
			//Check specific integers and make sure they're on the right line (not outside board)
			//Eight moves:
			//Y: +2, X: +1 (16+1 = 17)
			//Y: -2, X: +1 (-16+1 = -15)
			//Y: +2, X: -1 (16-1 = 15)
			//Y: -2, X: -1 (-16-1 = -17)
			//Y: +1, X: +2 (8+2 = 10)
			//Y: -1, X: +2 (-8+2 = -6)
			//Y: +1, X: -2 (8-2 = 6)
			//Y: -1, X: -2 (-8-2 = -10)
			if (i+17 < 64 && (board[i+17] == 0 || board[i+17] > 8) //17
				&& ((i + 17) / 8 == i / 8 + 2))
			{
				moves.push_back(Move::getInt16FromPos(i, i + 17));
			}
			if (i > 16 && (board[i - 17] == 0 || board[i - 17] > 8) //-17
				&& ((i - 17) / 8 == i / 8 - 2))
			{
				moves.push_back(Move::getInt16FromPos(i, i - 17));
			}
			if (i + 15 < 64 && (board[i + 15] == 0 || board[i + 15] > 8) //15
				&& ((i + 15) / 8 == i / 8 + 2))
			{
				moves.push_back(Move::getInt16FromPos(i, i + 15));
			}
			if (i > 14 && (board[i -15] == 0 || board[i - 15] > 8) //-15
				&& ((i - 15) / 8 == i / 8 - 2))
			{
				moves.push_back(Move::getInt16FromPos(i, i - 15));
			}
			if (i + 10 < 64 && (board[i + 10] == 0 || board[i + 10] > 8) //10
				&& ((i + 10) / 8 == i / 8 + 1))
			{
				moves.push_back(Move::getInt16FromPos(i, i + 10));
			}
			if (i > 9 && (board[i - 10] == 0 || board[i - 10] > 8) //-10
				&& ((i - 10) / 8 == i / 8 - 1))
			{
				moves.push_back(Move::getInt16FromPos(i, i - 10));
			}
			if (i + 6 < 64 && (board[i + 6] == 0 || board[i + 6] > 8) //6
				&& ((i + 6) / 8 == i / 8 + 1))
			{
				moves.push_back(Move::getInt16FromPos(i, i + 6));
			}
			if (i > 5 && (board[i - 6] == 0 || board[i - 6] > 8) //-6
				&& ((i - 6) / 8 == i / 8 - 1))
			{
				moves.push_back(Move::getInt16FromPos(i, i - 6));
			}
			break;
		case 4: //White bishop
			//Four directions: +9, +7, -9, -7
			//Need to do both y and x check all of them
			index = i + 9;
			while (index < 64 && index / 8 == (index - 9) / 8 + 1) //+9, outside board check
			{
				if (board[index] != 0)
				{
					if (board[index] > 8) //Can take
					{
						moves.push_back(Move::getInt16FromPos(i, index));
					}
					break; //Otherwise, stop looking
				}
				//Empty square, add move
				moves.push_back(Move::getInt16FromPos(i, index));
				index = index + 9;
			}
			index = i - 9;
			while (index > -1 && index / 8 == (index + 9) / 8 - 1) //-9, outside board check
			{
				if (board[index] != 0)
				{
					if (board[index] > 8) //Can take
					{
						moves.push_back(Move::getInt16FromPos(i, index));
					}
					break; //Otherwise, stop looking
				}
				//Empty square, add move
				moves.push_back(Move::getInt16FromPos(i, index));
				index = index - 9;
			}
			index = i + 7;
			while (index < 64 && index / 8 == (index - 7) / 8 + 1) //+7, outside board check
			{
				if (board[index] != 0)
				{
					if (board[index] > 8) //Can take
					{
						moves.push_back(Move::getInt16FromPos(i, index));
					}
					break; //Otherwise, stop looking
				}
				//Empty square, add move
				moves.push_back(Move::getInt16FromPos(i, index));
				index = index + 7;
			}
			index = i - 7;
			while (index > -1 && index / 8 == (index + 7) / 8 - 1) //-7, outside board check
			{
				if (board[index] != 0)
				{
					if (board[index] > 8) //Can take
					{
						moves.push_back(Move::getInt16FromPos(i, index));
					}
					break; //Otherwise, stop looking
				}
				//Empty square, add move
				moves.push_back(Move::getInt16FromPos(i, index));
				index = index - 7;
			}
			break;
		case 5: //White queen
			//Rook part
			index = i + 8;
			while (index < 64) //+8, outside board check
			{
				if (board[index] != 0)
				{
					if (board[index] > 8) //Can take
					{
						moves.push_back(Move::getInt16FromPos(i, index));
					}
					break; //Otherwise, stop looking
				}
				//Empty square, add move
				moves.push_back(Move::getInt16FromPos(i, index));
				index = index + 8;
			}
			index = i - 8;
			while (index > -1) //-8, outside board check
			{
				if (board[index] != 0)
				{
					if (board[index] > 8) //Can take
					{
						moves.push_back(Move::getInt16FromPos(i, index));
					}
					break; //Otherwise, stop looking
				}
				//Empty square, add move
				moves.push_back(Move::getInt16FromPos(i, index));
				index = index - 8;
			}
			index = i + 1;
			while (index / 8 == i / 8) //+8, outside board check
			{
				if (board[index] != 0)
				{
					if (board[index] > 8) //Can take
					{
						moves.push_back(Move::getInt16FromPos(i, index));
					}
					break; //Otherwise, stop looking
				}
				//Empty square, add move
				moves.push_back(Move::getInt16FromPos(i, index));
				index = index + 1;
			}
			index = i - 1;
			while (index > -1 && index / 8 == i / 8) //+8, outside board check
			{
				if (board[index] != 0)
				{
					if (board[index] > 8) //Can take
					{
						moves.push_back(Move::getInt16FromPos(i, index));
					}
					break; //Otherwise, stop looking
				}
				//Empty square, add move
				moves.push_back(Move::getInt16FromPos(i, index));
				index = index - 1;
			}
			//Bishop part
			index = i + 9;
			while (index < 64 && index / 8 == (index - 9) / 8 + 1) //+9, outside board check
			{
				if (board[index] != 0)
				{
					if (board[index] > 8) //Can take
					{
						moves.push_back(Move::getInt16FromPos(i, index));
					}
					break; //Otherwise, stop looking
				}
				//Empty square, add move
				moves.push_back(Move::getInt16FromPos(i, index));
				index = index + 9;
			}
			index = i - 9;
			while (index > -1 && index / 8 == (index + 9) / 8 - 1) //-9, outside board check
			{
				if (board[index] != 0)
				{
					if (board[index] > 8) //Can take
					{
						moves.push_back(Move::getInt16FromPos(i, index));
					}
					break; //Otherwise, stop looking
				}
				//Empty square, add move
				moves.push_back(Move::getInt16FromPos(i, index));
				index = index - 9;
			}
			index = i + 7;
			while (index < 64 && index / 8 == (index - 7) / 8 + 1) //+7, outside board check
			{
				if (board[index] != 0)
				{
					if (board[index] > 8) //Can take
					{
						moves.push_back(Move::getInt16FromPos(i, index));
					}
					break; //Otherwise, stop looking
				}
				//Empty square, add move
				moves.push_back(Move::getInt16FromPos(i, index));
				index = index + 7;
			}
			index = i - 7;
			while (index > -1 && index / 8 == (index + 7) / 8 - 1) //-7, outside board check
			{
				if (board[index] != 0)
				{
					if (board[index] > 8) //Can take
					{
						moves.push_back(Move::getInt16FromPos(i, index));
					}
					break; //Otherwise, stop looking
				}
				//Empty square, add move
				moves.push_back(Move::getInt16FromPos(i, index));
				index = index - 7;
			}
			break;
		case 6: //White king (add attacked_square support later)
			//Movements: +7,+8,+9,+1,-1,-9,-8,-7
			if (i > 0 && (board[i-1] == 0 || board[i-1] > 8) && (i - 1) / 8 == i / 8) //-1
			{
				if (!squareAttackedByBlack(i-1))
					moves.push_back(Move::getInt16FromPos(i, i - 1));
			}
			if (i > 8 && (board[i - 9] == 0 || board[i - 9] > 8) && (i - 9) / 8 == i / 8 - 1) //-9
			{
				if (!squareAttackedByBlack(i - 9))
					moves.push_back(Move::getInt16FromPos(i, i - 9));
			}
			if (i > 7 && (board[i - 8] == 0 || board[i - 8] > 8) && (i - 8) / 8 == i / 8 - 1) //-8
			{
				if (!squareAttackedByBlack(i - 8))
					moves.push_back(Move::getInt16FromPos(i, i - 8));
			}
			if (i > 6 && (board[i - 7] == 0 || board[i - 7] > 8) && (i - 7) / 8 == i / 8 - 1) //-7
			{
				if (!squareAttackedByBlack(i - 7))
					moves.push_back(Move::getInt16FromPos(i, i - 7));
			}
			if (i < 63 && (board[i + 1] == 0 || board[i + 1] > 8) && (i + 1) / 8 == i / 8) //+1
			{
				if (!squareAttackedByBlack(i + 1))
					moves.push_back(Move::getInt16FromPos(i, i + 1));
			}
			if (i < 55 && (board[i + 9] == 0 || board[i + 9] > 8) && (i + 9) / 8 == i / 8 + 1) //+9
			{
				if (!squareAttackedByBlack(i + 9))
					moves.push_back(Move::getInt16FromPos(i, i + 9));
			}
			if (i < 56 && (board[i + 8] == 0 || board[i + 8] > 8) && (i + 8) / 8 == i / 8 + 1) //+8
			{
				if (!squareAttackedByBlack(i + 8))
					moves.push_back(Move::getInt16FromPos(i, i + 8));
			}
			if (i < 57 && (board[i + 7] == 0 || board[i + 7] > 8) && (i + 7) / 8 == i / 8 + 1) //+7
			{
				if (!squareAttackedByBlack(i + 7))
					moves.push_back(Move::getInt16FromPos(i, i + 7));
			}
			break;
		}
	}
	//Generates En Passant moves (-1 for no en passant)
	if (enPassantSquare > 0) //This means that white can perform en passant
	{
		if (((enPassantSquare + 9) / 8 == (enPassantSquare / 8) + 1) && board[enPassantSquare + 9] == 1)
		{
			//White pawn can take en passant square
			board[enPassantSquare + 9] = 0;
			board[enPassantSquare + 8] = 0;
			board[enPassantSquare] = 1;

			if (!squareAttackedByBlack(kingIndex))
				moves.push_back(Move::getInt16FromPos(enPassantSquare + 9, enPassantSquare));
			board[enPassantSquare + 9] = 1;
			board[enPassantSquare + 8] = 11;
			board[enPassantSquare] = 0;
		}
		if (((enPassantSquare + 7) / 8 == (enPassantSquare / 8) + 1) && board[enPassantSquare + 7] == 1)
		{
			//White pawn can take en passant square
			board[enPassantSquare + 7] = 0;
			board[enPassantSquare + 8] = 0;
			board[enPassantSquare] = 1;

			if (!squareAttackedByBlack(kingIndex))
				moves.push_back(Move::getInt16FromPos(enPassantSquare + 7, enPassantSquare));
			board[enPassantSquare + 7] = 1;
			board[enPassantSquare + 8] = 11;
			board[enPassantSquare] = 0;
		}
	}
	
	//Generates castling
	if (castlingWhiteKingSide)
	{
		if (board[62] == 0 && board[61] == 0) //If the squares are empty
		{
			if (!squareAttackedByBlack(62) && !squareAttackedByBlack(61))
				moves.push_back(Move::getInt16FromPos(60, 62));
		}
	}
	if (castlingWhiteQueenSide)
	{
		if (board[59] == 0 && board[58] == 0 && board[57] == 0) //If the squares are empty
		{
			if (!squareAttackedByBlack(59) && !squareAttackedByBlack(58))
				moves.push_back(Move::getInt16FromPos(60, 58));
		}
	}
	//Finally, the moves are generated and can be returned fully legal
	return moves;
}

vector<__int16> ArrayBoard::generateBlackLegalMoves()
{
	//Idea: use two vectors, one for attack and one for quiet moves for move ordering?
	vector<__int16> moves;
	moves.reserve(32);
	vector<short> ignorePiecesSet;
	int index;
	//Check for pinned pieces for king safety reasons
	//Check whether a black sliding piece can attack the black king
	//If they can, look if a piece is pinned.
	//	If a piece is pinned, generate moves only along that ray if possible
	//	Add to skipSet to remove from search
	int kingIndex;
	for (size_t i = 0; i < 64; i++)  //Find the king
	{
		if (board[i] == 16) //Found him!
		{
			kingIndex = i;
			break;
		}
	}

	int kingColumn = kingIndex % 8;
	int kingRow = kingIndex / 8;

	//Declare a variable to store a possible king attacker.
	//-1 means no attacker
	int kingAttackerIndex = -1;

	//Check if king is threatened by pawn or bishop
	//Pawn moves threatening king
	if (kingRow < 7 && board[kingIndex + 9] == 1 && diagonalArray[kingIndex] == diagonalArray[kingIndex + 9])
		kingAttackerIndex = kingIndex + 9;
	if (kingRow < 7 && board[kingIndex + 7] == 1 && antiDiagonalArray[kingIndex] == antiDiagonalArray[kingIndex + 7])
	{
		if (kingAttackerIndex < 0)
			kingAttackerIndex = kingIndex + 7; //Assign attacker
		else
			return generateBlackEvasionMoves(moves, -1, kingIndex, ignorePiecesSet); //Two attackers
	}
	//Knights threatening king
	if (kingIndex + 17 < 64 && (board[kingIndex + 17] == 3) //17
		&& ((kingIndex + 17) / 8 == kingRow + 2))
	{
		if (kingAttackerIndex < 0)
			kingAttackerIndex = kingIndex + 17; //Assign attacker
		else
			return generateBlackEvasionMoves(moves, -1, kingIndex, ignorePiecesSet); //Two attackers
	}
	if (kingIndex > 16 && (board[kingIndex - 17] == 3) //-17
		&& ((kingIndex - 17) / 8 == kingRow - 2))
	{
		if (kingAttackerIndex < 0)
			kingAttackerIndex = kingIndex - 17; //Assign attacker
		else
			return generateBlackEvasionMoves(moves, -1, kingIndex, ignorePiecesSet); //Two attackers
	}
	if (kingIndex + 15 < 64 && (board[kingIndex + 15] == 3) //15
		&& ((kingIndex + 15) / 8 == kingRow + 2))
	{
		if (kingAttackerIndex < 0)
			kingAttackerIndex = kingIndex + 15; //Assign attacker
		else
			return generateBlackEvasionMoves(moves, -1, kingIndex, ignorePiecesSet); //Two attackers
	}
	if (kingIndex > 14 && (board[kingIndex - 15] == 3) //-15
		&& ((kingIndex - 15) / 8 == kingRow - 2))
	{
		if (kingAttackerIndex < 0)
			kingAttackerIndex = kingIndex - 15; //Assign attacker
		else
			return generateBlackEvasionMoves(moves, -1, kingIndex, ignorePiecesSet); //Two attackers
	}
	if (kingIndex + 10 < 64 && (board[kingIndex + 10] == 3) //10
		&& ((kingIndex + 10) / 8 == kingRow + 1))
	{
		if (kingAttackerIndex < 0)
			kingAttackerIndex = kingIndex + 10; //Assign attacker
		else
			return generateBlackEvasionMoves(moves, -1, kingIndex, ignorePiecesSet); //Two attackers
	}
	if (kingIndex > 9 && (board[kingIndex - 10] == 3) //-10
		&& ((kingIndex - 10) / 8 == kingRow - 1))
	{
		if (kingAttackerIndex < 0)
			kingAttackerIndex = kingIndex - 10; //Assign attacker
		else
			return generateBlackEvasionMoves(moves, -1, kingIndex, ignorePiecesSet); //Two attackers
	}
	if (kingIndex + 6 < 64 && (board[kingIndex + 6] == 3) //6
		&& ((kingIndex + 6) / 8 == kingRow + 1))
	{
		if (kingAttackerIndex < 0)
			kingAttackerIndex = kingIndex + 6; //Assign attacker
		else
			return generateBlackEvasionMoves(moves, -1, kingIndex, ignorePiecesSet); //Two attackers
	}
	if (kingIndex > 5 && (board[kingIndex - 6] == 3) //-6
		&& ((kingIndex - 6) / 8 == kingRow - 1))
		{
			if (kingAttackerIndex < 0)
				kingAttackerIndex = kingIndex - 6; //Assign attacker
			else
				return generateBlackEvasionMoves(moves, -1, kingIndex, ignorePiecesSet); //Two attackers
		}

	bool noPinnedPiece = true;
	int pinIndex;
	//Generate pinned pieces
	for (size_t i = 0; i < 64; i++) //Find black sliding pieces
	{
		//Check whether they align with the white king
		switch (board[i])
		{
		case 2: //Rook [FULLY FUNCTIONAL]
			if (i % 8 == kingColumn) //Same column
			{
				//King could be threatened by this piece. Do raycast
				//Check if king is above or below
				if (i > kingIndex) //Black rook moving UP
				{
					index = i - 8;
					//Check move until king is reached. There has to be a piece inbetween somewhere
					while (index != kingIndex)
					{
						if (board[index] == 0) //If it encounters nothing
						{
						}
						else if (board[index] < 8) //If it is a piece of its own color, break
						{
							noPinnedPiece = true;
							break;
						}
						else if (board[index] > 8) //Black piece that is pinned! Jackpot
						{
							if (noPinnedPiece) //Found a pinned piece! Create moves here.
							{
								pinIndex = index;
								noPinnedPiece = false;
							}
							else //Discovered another black piece, no longer a pin. Quit yer search!
							{
								noPinnedPiece = true;
								break;
							}
						}
						index = index - 8;
					}
					if (!noPinnedPiece)
					{

						//100 % certain we found a pinned piece
						//Generate moves for either rook or queen moving DOWN
						ignorePiecesSet.push_back(pinIndex);
						if (board[pinIndex] == 12 || board[pinIndex] == 15)
						{
							index = kingIndex;
							while (index != i)
							{
								index = index + 8;
								if (index != pinIndex)
								{
									moves.push_back(Move::getInt16FromPos(pinIndex, index));
								}
							}
						}
						else if (board[pinIndex] == 11)
						{
							if (pinIndex > 7 && pinIndex < 15 && pinIndex + 16 < i) //Can doublemove and singlemove
							{
								moves.push_back(Move::getInt16FromPos(pinIndex, pinIndex + 16));
								moves.push_back(Move::getInt16FromPos(pinIndex, pinIndex + 8));
							}
							else if (pinIndex + 8 < i) //Can singlemove
								moves.push_back(Move::getInt16FromPos(pinIndex, pinIndex + 8));
						}
						noPinnedPiece = true;
						continue; //No need to check other direction, there's only 1 king
					}
					else if (index == kingIndex)
					{
						//No piece inbetween! The piece is in check!
						if (kingAttackerIndex < 0)
							kingAttackerIndex = i; //Assign attacker
						else
							return generateBlackEvasionMoves(moves, -1, kingIndex, ignorePiecesSet); //Two attackers
					}
				}
				else //Other direction, black rook moving DOWN
				{
					index = i + 8;
					//Check move until king is reached. There has to be a piece inbetween somewhere
					while (index != kingIndex)
					{
						if (board[index] == 0) //If it encounters nothing
						{
						}
						else if (board[index] < 8) //If it is a piece of its own color, break
						{
							noPinnedPiece = true;
							break;
						}
						else if (board[index] > 8) //White piece that is pinned! Jackpot
						{
							if (noPinnedPiece) //Found a pinned piece! Create moves here.
							{
								pinIndex = index;
								noPinnedPiece = false;
							}
							else //Discovered another white piece, no longer a pin. Quit yer search!
							{
								noPinnedPiece = true;
								break;
							}
						}
						index = index + 8;
					}
					if (!noPinnedPiece)
					{

						//100 % certain we found a pinned piece
						//Generate moves for either rook or queen moving UP
						ignorePiecesSet.push_back(pinIndex);
						if (board[pinIndex] == 12 || board[pinIndex] == 15)
						{
							index = kingIndex;
							while (index != i)
							{
								index = index - 8;
								if (index != pinIndex)
								{
									moves.push_back(Move::getInt16FromPos(pinIndex, index));
								}
							}
						}
						else if (board[pinIndex] == 11)
						{
							if (pinIndex > 7 && pinIndex < 15 && pinIndex + 16 < kingIndex) //Can doublemove and singlemove
							{
								moves.push_back(Move::getInt16FromPos(pinIndex, pinIndex + 16));
								moves.push_back(Move::getInt16FromPos(pinIndex, pinIndex + 8));
							}
							else if (pinIndex + 8 < kingIndex) //Can singlemove
								moves.push_back(Move::getInt16FromPos(pinIndex, pinIndex + 8));
						}
						noPinnedPiece = true;
						continue; //No need to check other direction, there's only 1 king
					}
					else if (index == kingIndex)
					{
						//No piece inbetween! The piece is in check!
						if (kingAttackerIndex < 0)
							kingAttackerIndex = i; //Assign attacker
						else
							return generateBlackEvasionMoves(moves, -1, kingIndex, ignorePiecesSet); //Two attackers
					}
				}

			}
			else if (i / 8 == kingRow) //Same row
			{
				//King could be threatened by this piece. Do raycast
				//Check if king is above or below
				if (i > kingIndex) //Black rook moving LEFT
				{
					index = i - 1;
					//Check move until king is reached. There has to be a piece inbetween somewhere
					while (index != kingIndex)
					{
						if (board[index] == 0) //If it encounters nothing
						{
						}
						else if (board[index] < 8) //If it is a piece of its own color, break
						{
							noPinnedPiece = true;
							break;
						}
						else if (board[index] > 8) //White piece that is pinned! Jackpot
						{
							if (noPinnedPiece) //Found a pinned piece! Create moves here.
							{
								pinIndex = index;
								noPinnedPiece = false;
							}
							else //Discovered another white piece, no longer a pin. Quit yer search!
							{
								noPinnedPiece = true;
								break;
							}
						}
						index = index - 1;
					}
					if (!noPinnedPiece)
					{

						//100 % certain we found a pinned piece
						//Generate moves for either rook or queen moving RIGHT
						ignorePiecesSet.push_back(pinIndex);
						if (board[pinIndex] == 12 || board[pinIndex] == 15)
						{
							index = kingIndex;
							while (index != i)
							{
								index = index + 1;
								if (index != pinIndex)
								{
									moves.push_back(Move::getInt16FromPos(pinIndex, index));
								}
							}
						}
						noPinnedPiece = true;
						continue; //No need to check other direction, there's only 1 king
					}
					else if (index == kingIndex)
					{
						//No piece inbetween! The piece is in check!
						if (kingAttackerIndex < 0)
							kingAttackerIndex = i; //Assign attacker
						else
							return generateBlackEvasionMoves(moves, -1, kingIndex, ignorePiecesSet); //Two attackers
					}
				}
				else //Other direction, black rook moving RIGHT
				{
					index = i + 1;
					//Check move until king is reached. There has to be a piece inbetween somewhere
					while (index != kingIndex)
					{
						if (board[index] == 0) //If it encounters nothing
						{
						}
						else if (board[index] < 8) //If it is a piece of its own color, break
						{
							noPinnedPiece = true;
							break;
						}
						else if (board[index] > 8) //White piece that is pinned! Jackpot
						{
							if (noPinnedPiece) //Found a pinned piece! Create moves here.
							{
								pinIndex = index;
								noPinnedPiece = false;
							}
							else //Discovered another white piece, no longer a pin. Quit yer search!
							{
								noPinnedPiece = true;
								break;
							}
						}
						index = index + 1;
					}
					if (!noPinnedPiece)
					{

						//100 % certain we found a pinned piece
						//Generate moves for either rook or queen moving UP
						ignorePiecesSet.push_back(pinIndex);
						if (board[pinIndex] == 12 || board[pinIndex] == 15)
						{
							index = kingIndex;
							while (index != i)
							{
								index = index - 1;
								if (index != pinIndex)
								{
									moves.push_back(Move::getInt16FromPos(pinIndex, index));
								}
							}
						}
						noPinnedPiece = true;
						continue; //No need to check other direction, there's only 1 king
					}
					else if (index == kingIndex)
					{
						//No piece inbetween! The piece is in check!
						if (kingAttackerIndex < 0)
							kingAttackerIndex = i; //Assign attacker
						else
							return generateBlackEvasionMoves(moves, -1, kingIndex, ignorePiecesSet); //Two attackers
					}
				}
			}
			break;
		case 4: //Bishop
			if (diagonalArray[i] == diagonalArray[kingIndex]) //Diagonal: main file a1h8
			{
				if (i > kingIndex) //Black bishop moving diagonal UP
				{
					index = i - 9;
					//Check move until king is reached. There has to be a piece inbetween somewhere
					while (index != kingIndex)
					{
						if (board[index] == 0) //If it encounters nothing
						{
						}
						else if (board[index] < 8) //If it is a piece of its own color, break
						{
							noPinnedPiece = true;
							break;
						}
						else if (board[index] > 8) //White piece that is pinned! Jackpot
						{
							if (noPinnedPiece) //Found a pinned piece! Create moves here.
							{
								pinIndex = index;
								noPinnedPiece = false;
							}
							else //Discovered another white piece, no longer a pin. Quit yer search!
							{
								noPinnedPiece = true;
								break;
							}
						}
						index = index - 9;
					}
					if (!noPinnedPiece)
					{

						//100 % certain we found a pinned piece
						//Generate moves for either rook or queen moving DOWN
						ignorePiecesSet.push_back(pinIndex);
						if (board[pinIndex] == 14 || board[pinIndex] == 15)
						{
							index = kingIndex;
							while (index != i)
							{
								index = index + 9;
								if (index != pinIndex)
								{
									moves.push_back(Move::getInt16FromPos(pinIndex, index));
								}
							}
						}
						else if (board[pinIndex] == 11) //Pawn, check if it can take directly
						{
							if (i == pinIndex + 9)
							{
								moves.push_back(Move::getInt16FromPos(pinIndex, i));
							}
						}
						noPinnedPiece = true;
						continue; //No need to check other direction, there's only 1 king
					}
					else if (index == kingIndex)
					{
						//No piece inbetween! The piece is in check!
						if (kingAttackerIndex < 0)
							kingAttackerIndex = i; //Assign attacker
						else
							return generateBlackEvasionMoves(moves, -1, kingIndex, ignorePiecesSet); //Two attackers
					}
				}
				else //Other direction, black rook moving DOWN
				{
					index = i + 9;
					//Check move until king is reached. There has to be a piece inbetween somewhere
					while (index != kingIndex)
					{
						if (board[index] == 0) //If it encounters nothing
						{
						}
						else if (board[index] < 8) //If it is a piece of its own color, break
						{
							noPinnedPiece = true;
							break;
						}
						else if (board[index] > 8) //White piece that is pinned! Jackpot
						{
							if (noPinnedPiece) //Found a pinned piece! Create moves here.
							{
								pinIndex = index;
								noPinnedPiece = false;
							}
							else //Discovered another white piece, no longer a pin. Quit yer search!
							{
								noPinnedPiece = true;
								break;
							}
						}
						index = index + 9;
					}
					if (!noPinnedPiece)
					{

						//100 % certain we found a pinned piece
						//Generate moves for either rook or queen moving UP
						ignorePiecesSet.push_back(pinIndex);
						if (board[pinIndex] == 14 || board[pinIndex] == 15)
						{
							index = kingIndex;
							while (index != i)
							{
								index = index - 9;
								if (index != pinIndex)
								{
									moves.push_back(Move::getInt16FromPos(pinIndex, index));
								}
							}
						}
						noPinnedPiece = true;
						continue; //No need to check other direction, there's only 1 king
					}
					else if (index == kingIndex)
					{
						//No piece inbetween! The piece is in check!
						if (kingAttackerIndex < 0)
							kingAttackerIndex = i; //Assign attacker
						else
							return generateBlackEvasionMoves(moves, -1, kingIndex, ignorePiecesSet); //Two attackers
					}
				}

			}
			else if (antiDiagonalArray[i] == antiDiagonalArray[kingIndex])
			{
				if (i > kingIndex) //Black bishop moving diagonal UP
				{
					index = i - 7;
					//Check move until king is reached. There has to be a piece inbetween somewhere
					while (index != kingIndex)
					{
						if (board[index] == 0) //If it encounters nothing
						{
						}
						else if (board[index] < 8) //If it is a piece of its own color, break
						{
							noPinnedPiece = true;
							break;
						}
						else if (board[index] > 8) //White piece that is pinned! Jackpot
						{
							if (noPinnedPiece) //Found a pinned piece! Create moves here.
							{
								pinIndex = index;
								noPinnedPiece = false;
							}
							else //Discovered another white piece, no longer a pin. Quit yer search!
							{
								noPinnedPiece = true;
								break;
							}
						}
						index = index - 7;
					}
					if (!noPinnedPiece)
					{

						//100 % certain we found a pinned piece
						//Generate moves for either rook or queen moving DOWN
						ignorePiecesSet.push_back(pinIndex);
						if (board[pinIndex] == 14 || board[pinIndex] == 15)
						{
							index = kingIndex;
							while (index != i)
							{
								index = index + 7;
								if (index != pinIndex)
								{
									moves.push_back(Move::getInt16FromPos(pinIndex, index));
								}
							}
						}
						else if (board[pinIndex] == 11) //Pawn, check if it can take directly
						{
							if (i == pinIndex + 7)
							{
								moves.push_back(Move::getInt16FromPos(pinIndex, i));
							}
						}
						noPinnedPiece = true;
						continue; //No need to check other direction, there's only 1 king
					}
					else if (index == kingIndex)
					{
						//No piece inbetween! The piece is in check!
						if (kingAttackerIndex < 0)
							kingAttackerIndex = i; //Assign attacker
						else
							return generateBlackEvasionMoves(moves, -1, kingIndex, ignorePiecesSet); //Two attackers
					}
				}
				else //Other direction, black rook moving DOWN
				{
					index = i + 7;
					//Check move until king is reached. There has to be a piece inbetween somewhere
					while (index != kingIndex)
					{
						if (board[index] == 0) //If it encounters nothing
						{
						}
						else if (board[index] < 8) //If it is a piece of its own color, break
						{
							noPinnedPiece = true;
							break;
						}
						else if (board[index] > 8) //White piece that is pinned! Jackpot
						{
							if (noPinnedPiece) //Found a pinned piece! Create moves here.
							{
								pinIndex = index;
								noPinnedPiece = false;
							}
							else //Discovered another white piece, no longer a pin. Quit yer search!
							{
								noPinnedPiece = true;
								break;
							}
						}
						index = index + 7;
					}
					if (!noPinnedPiece)
					{

						//100 % certain we found a pinned piece
						//Generate moves for either rook or queen moving UP
						ignorePiecesSet.push_back(pinIndex);
						if (board[pinIndex] == 14 || board[pinIndex] == 15)
						{
							index = kingIndex;
							while (index != i)
							{
								index = index - 7;
								if (index != pinIndex)
								{
									moves.push_back(Move::getInt16FromPos(pinIndex, index));
								}
							}
						}
						noPinnedPiece = true;
						continue; //No need to check other direction, there's only 1 king
					}
					else if (index == kingIndex)
					{
						//No piece inbetween! The piece is in check!
						if (kingAttackerIndex < 0)
							kingAttackerIndex = i; //Assign attacker
						else
							return generateBlackEvasionMoves(moves, -1, kingIndex, ignorePiecesSet); //Two attackers
					}
				}

			}
			break;
		case 5: //Queen
			if (i % 8 == kingColumn) //Same column
			{
				//King could be threatened by this piece. Do raycast
				//Check if king is above or below
				if (i > kingIndex) //Black queen moving UP
				{
					index = i - 8;
					//Check move until king is reached. There has to be a piece inbetween somewhere
					while (index != kingIndex)
					{
						if (board[index] == 0) //If it encounters nothing
						{
						}
						else if (board[index] < 8) //If it is a piece of its own color, break
						{
							noPinnedPiece = true;
							break;
						}
						else if (board[index] > 8) //White piece that is pinned! Jackpot
						{
							if (noPinnedPiece) //Found a pinned piece! Create moves here.
							{
								pinIndex = index;
								noPinnedPiece = false;
							}
							else //Discovered another white piece, no longer a pin. Quit yer search!
							{
								noPinnedPiece = true;
								break;
							}
						}
						index = index - 8;
					}
					if (!noPinnedPiece)
					{

						//100 % certain we found a pinned piece
						//Generate moves for either rook or queen moving DOWN
						ignorePiecesSet.push_back(pinIndex);
						if (board[pinIndex] == 12 || board[pinIndex] == 15)
						{
							index = kingIndex;
							while (index != i)
							{
								index = index + 8;
								if (index != pinIndex)
								{
									moves.push_back(Move::getInt16FromPos(pinIndex, index));
								}
							}
						}
						else if (board[pinIndex] == 11)
						{
							if (pinIndex > 7 && pinIndex < 15 && pinIndex + 16 < i) //Can doublemove and singlemove
							{
								moves.push_back(Move::getInt16FromPos(pinIndex, pinIndex + 16));
								moves.push_back(Move::getInt16FromPos(pinIndex, pinIndex + 8));
							}
							else if (pinIndex + 8 < i) //Can singlemove
								moves.push_back(Move::getInt16FromPos(pinIndex, pinIndex + 8));
						}
						noPinnedPiece = true;
						continue; //No need to check other direction, there's only 1 king
					}
					else if (index == kingIndex)
					{
						//No piece inbetween! The piece is in check!
						if (kingAttackerIndex < 0)
							kingAttackerIndex = i; //Assign attacker
						else
							return generateBlackEvasionMoves(moves, -1, kingIndex, ignorePiecesSet); //Two attackers
					}
				}
				else //Other direction, black queen moving DOWN
				{
					index = i + 8;
					//Check move until king is reached. There has to be a piece inbetween somewhere
					while (index != kingIndex)
					{
						if (board[index] == 0) //If it encounters nothing
						{
						}
						else if (board[index] < 8) //If it is a piece of its own color, break
						{
							noPinnedPiece = true;
							break;
						}
						else if (board[index] > 8) //White piece that is pinned! Jackpot
						{
							if (noPinnedPiece) //Found a pinned piece! Create moves here.
							{
								pinIndex = index;
								noPinnedPiece = false;
							}
							else //Discovered another white piece, no longer a pin. Quit yer search!
							{
								noPinnedPiece = true;
								break;
							}
						}
						index = index + 8;
					}
					if (!noPinnedPiece)
					{

						//100 % certain we found a pinned piece
						//Generate moves for either rook or queen moving UP
						ignorePiecesSet.push_back(pinIndex);
						if (board[pinIndex] == 12 || board[pinIndex] == 15)
						{
							index = kingIndex;
							while (index != i)
							{
								index = index - 8;
								if (index != pinIndex)
								{
									moves.push_back(Move::getInt16FromPos(pinIndex, index));
								}
							}
						}
						else if (board[pinIndex] == 11)
						{
							if (pinIndex > 7 && pinIndex < 15 && pinIndex + 16 < kingIndex) //Can doublemove and singlemove
							{
								moves.push_back(Move::getInt16FromPos(pinIndex, pinIndex + 16));
								moves.push_back(Move::getInt16FromPos(pinIndex, pinIndex + 8));
							}
							else if (pinIndex + 8 < kingIndex) //Can singlemove
								moves.push_back(Move::getInt16FromPos(pinIndex, pinIndex + 8));
						}
						noPinnedPiece = true;
						continue; //No need to check other direction, there's only 1 king
					}
					else if (index == kingIndex)
					{
						//No piece inbetween! The piece is in check!
						if (kingAttackerIndex < 0)
							kingAttackerIndex = i; //Assign attacker
						else
							return generateBlackEvasionMoves(moves, -1, kingIndex, ignorePiecesSet); //Two attackers
					}
				}

			}
			else if (i / 8 == kingRow) //Same row
			{
				//King could be threatened by this piece. Do raycast
				//Check if king is above or below
				if (i > kingIndex) //Black queen moving LEFT
				{
					index = i - 1;
					//Check move until king is reached. There has to be a piece inbetween somewhere
					while (index != kingIndex)
					{
						if (board[index] == 0) //If it encounters nothing
						{
						}
						else if (board[index] < 8) //If it is a piece of its own color, break
						{
							noPinnedPiece = true;
							break;
						}
						else if (board[index] > 8) //White piece that is pinned! Jackpot
						{
							if (noPinnedPiece) //Found a pinned piece! Create moves here.
							{
								pinIndex = index;
								noPinnedPiece = false;
							}
							else //Discovered another white piece, no longer a pin. Quit yer search!
							{
								noPinnedPiece = true;
								break;
							}
						}
						index = index - 1;
					}
					if (!noPinnedPiece)
					{

						//100 % certain we found a pinned piece
						//Generate moves for either rook or queen moving RIGHT
						ignorePiecesSet.push_back(pinIndex);
						if (board[pinIndex] == 12 || board[pinIndex] == 15)
						{
							index = kingIndex;
							while (index != i)
							{
								index = index + 1;
								if (index != pinIndex)
								{
									moves.push_back(Move::getInt16FromPos(pinIndex, index));
								}
							}
						}
						noPinnedPiece = true;
						continue; //No need to check other direction, there's only 1 king
					}
					else if (index == kingIndex)
					{
						//No piece inbetween! The piece is in check!
						if (kingAttackerIndex < 0)
							kingAttackerIndex = i; //Assign attacker
						else
							return generateBlackEvasionMoves(moves, -1, kingIndex, ignorePiecesSet); //Two attackers
					}
				}
				else //Other direction, black queen moving RIGHT
				{
					index = i + 1;
					//Check move until king is reached. There has to be a piece inbetween somewhere
					while (index != kingIndex)
					{
						if (board[index] == 0) //If it encounters nothing
						{
						}
						else if (board[index] < 8) //If it is a piece of its own color, break
						{
							noPinnedPiece = true;
							break;
						}
						else if (board[index] > 8) //White piece that is pinned! Jackpot
						{
							if (noPinnedPiece) //Found a pinned piece! Create moves here.
							{
								pinIndex = index;
								noPinnedPiece = false;
							}
							else //Discovered another white piece, no longer a pin. Quit yer search!
							{
								noPinnedPiece = true;
								break;
							}
						}
						index = index + 1;
					}
					if (!noPinnedPiece)
					{

						//100 % certain we found a pinned piece
						//Generate moves for either rook or queen moving UP
						ignorePiecesSet.push_back(pinIndex);
						if (board[pinIndex] == 12 || board[pinIndex] == 15)
						{
							index = kingIndex;
							while (index != i)
							{
								index = index - 1;
								if (index != pinIndex)
								{
									moves.push_back(Move::getInt16FromPos(pinIndex, index));
								}
							}
						}
						noPinnedPiece = true;
						continue; //No need to check other direction, there's only 1 king
					}
					else if (index == kingIndex)
					{
						//No piece inbetween! The piece is in check!
						if (kingAttackerIndex < 0)
							kingAttackerIndex = i; //Assign attacker
						else
							return generateBlackEvasionMoves(moves, -1, kingIndex, ignorePiecesSet); //Two attackers
					}
				}
			}
			else if (diagonalArray[i] == diagonalArray[kingIndex]) //Diagonal: main file a1h8
			{
				if (i > kingIndex) //Black bishop moving diagonal UP
				{
					index = i - 9;
					//Check move until king is reached. There has to be a piece inbetween somewhere
					while (index != kingIndex)
					{
						if (board[index] == 0) //If it encounters nothing
						{
						}
						else if (board[index] < 8) //If it is a piece of its own color, break
						{
							noPinnedPiece = true;
							break;
						}
						else if (board[index] > 8) //White piece that is pinned! Jackpot
						{
							if (noPinnedPiece) //Found a pinned piece! Create moves here.
							{
								pinIndex = index;
								noPinnedPiece = false;
							}
							else //Discovered another white piece, no longer a pin. Quit yer search!
							{
								noPinnedPiece = true;
								break;
							}
						}
						index = index - 9;
					}
					if (!noPinnedPiece)
					{

						//100 % certain we found a pinned piece
						//Generate moves for either rook or queen moving DOWN
						ignorePiecesSet.push_back(pinIndex);
						if (board[pinIndex] == 14 || board[pinIndex] == 15)
						{
							index = kingIndex;
							while (index != i)
							{
								index = index + 9;
								if (index != pinIndex)
								{
									moves.push_back(Move::getInt16FromPos(pinIndex, index));
								}
							}
						}
						else if (board[pinIndex] == 11) //Pawn, check if it can take directly
						{
							if (i == pinIndex + 9)
							{
								moves.push_back(Move::getInt16FromPos(pinIndex, i));
							}
						}
						noPinnedPiece = true;
						continue; //No need to check other direction, there's only 1 king
					}
					else if (index == kingIndex)
					{
						//No piece inbetween! The piece is in check!
						if (kingAttackerIndex < 0)
							kingAttackerIndex = i; //Assign attacker
						else
							return generateBlackEvasionMoves(moves, -1, kingIndex, ignorePiecesSet); //Two attackers
					}
				}
				else //Other direction, black rook moving DOWN
				{
					index = i + 9;
					//Check move until king is reached. There has to be a piece inbetween somewhere
					while (index != kingIndex)
					{
						if (board[index] == 0) //If it encounters nothing
						{
						}
						else if (board[index] < 8) //If it is a piece of its own color, break
						{
							noPinnedPiece = true;
							break;
						}
						else if (board[index] > 8) //White piece that is pinned! Jackpot
						{
							if (noPinnedPiece) //Found a pinned piece! Create moves here.
							{
								pinIndex = index;
								noPinnedPiece = false;
							}
							else //Discovered another white piece, no longer a pin. Quit yer search!
							{
								noPinnedPiece = true;
								break;
							}
						}
						index = index + 9;
					}
					if (!noPinnedPiece)
					{

						//100 % certain we found a pinned piece
						//Generate moves for either rook or queen moving UP
						ignorePiecesSet.push_back(pinIndex);
						if (board[pinIndex] == 14 || board[pinIndex] == 15)
						{
							index = kingIndex;
							while (index != i)
							{
								index = index - 9;
								if (index != pinIndex)
								{
									moves.push_back(Move::getInt16FromPos(pinIndex, index));
								}
							}
						}
						noPinnedPiece = true;
						continue; //No need to check other direction, there's only 1 king
					}
					else if (index == kingIndex)
					{
						//No piece inbetween! The piece is in check!
						if (kingAttackerIndex < 0)
							kingAttackerIndex = i; //Assign attacker
						else
							return generateBlackEvasionMoves(moves, -1, kingIndex, ignorePiecesSet); //Two attackers
					}
				}

			}
			else if (antiDiagonalArray[i] == antiDiagonalArray[kingIndex])
			{
				if (i > kingIndex) //Black bishop moving diagonal UP
				{
					index = i - 7;
					//Check move until king is reached. There has to be a piece inbetween somewhere
					while (index != kingIndex)
					{
						if (board[index] == 0) //If it encounters nothing
						{
						}
						else if (board[index] < 8) //If it is a piece of its own color, break
						{
							noPinnedPiece = true;
							break;
						}
						else if (board[index] > 8) //White piece that is pinned! Jackpot
						{
							if (noPinnedPiece) //Found a pinned piece! Create moves here.
							{
								pinIndex = index;
								noPinnedPiece = false;
							}
							else //Discovered another white piece, no longer a pin. Quit yer search!
							{
								noPinnedPiece = true;
								break;
							}
						}
						index = index - 7;
					}
					if (!noPinnedPiece)
					{

						//100 % certain we found a pinned piece
						//Generate moves for either rook or queen moving DOWN
						ignorePiecesSet.push_back(pinIndex);
						if (board[pinIndex] == 14 || board[pinIndex] == 15)
						{
							index = kingIndex;
							while (index != i)
							{
								index = index + 7;
								if (index != pinIndex)
								{
									moves.push_back(Move::getInt16FromPos(pinIndex, index));
								}
							}
						}
						else if (board[pinIndex] == 11) //Pawn, check if it can take directly
						{
							if (i == pinIndex + 7)
							{
								moves.push_back(Move::getInt16FromPos(pinIndex, i));
							}
						}
						noPinnedPiece = true;
						continue; //No need to check other direction, there's only 1 king
					}
					else if (index == kingIndex)
					{
						//No piece inbetween! The piece is in check!
						if (kingAttackerIndex < 0)
							kingAttackerIndex = i; //Assign attacker
						else
							return generateBlackEvasionMoves(moves, -1, kingIndex, ignorePiecesSet); //Two attackers
					}
				}
				else //Other direction, black rook moving DOWN
				{
					index = i + 7;
					//Check move until king is reached. There has to be a piece inbetween somewhere
					while (index != kingIndex)
					{
						if (board[index] == 0) //If it encounters nothing
						{
						}
						else if (board[index] < 8) //If it is a piece of its own color, break
						{
							noPinnedPiece = true;
							break;
						}
						else if (board[index] > 8) //White piece that is pinned! Jackpot
						{
							if (noPinnedPiece) //Found a pinned piece! Create moves here.
							{
								pinIndex = index;
								noPinnedPiece = false;
							}
							else //Discovered another white piece, no longer a pin. Quit yer search!
							{
								noPinnedPiece = true;
								break;
							}
						}
						index = index + 7;
					}
					if (!noPinnedPiece)
					{

						//100 % certain we found a pinned piece
						//Generate moves for either rook or queen moving UP
						ignorePiecesSet.push_back(pinIndex);
						if (board[pinIndex] == 14 || board[pinIndex] == 15)
						{
							index = kingIndex;
							while (index != i)
							{
								index = index - 7;
								if (index != pinIndex)
								{
									moves.push_back(Move::getInt16FromPos(pinIndex, index));
								}
							}
						}
						else if (board[pinIndex] == 11) //Pawn, check if it can take directly
						{
							if (i == pinIndex - 7)
							{
								moves.push_back(Move::getInt16FromPos(pinIndex, i));
							}
						}
						noPinnedPiece = true;
						continue; //No need to check other direction, there's only 1 king
					}
					else if (index == kingIndex)
					{
						//No piece inbetween! The piece is in check!
						if (kingAttackerIndex < 0)
							kingAttackerIndex = i; //Assign attacker
						else
							return generateBlackEvasionMoves(moves, -1, kingIndex, ignorePiecesSet); //Two attackers
					}
				}

			}
			break;
		}
	}
	
	//Check for check :)
	if (!(kingAttackerIndex < 0))
	{
		//Generate white evasion moves
		return generateBlackEvasionMoves(moves, kingAttackerIndex, kingIndex, ignorePiecesSet);
	}
	
	//Generate main moves!
	for (size_t i = 0; i < 64; i++)
	{
		if (find(ignorePiecesSet.begin(), ignorePiecesSet.end(), i) != ignorePiecesSet.end())
			continue;
		switch (board[i]) //Switch to allow for compiler optimisation
		{
		case 0: //Do nothing
			break;
		case 11: //Black pawn
			if (board[i + 8] == 0) //Move forward
			{
				if (i > 47) //Promotion straight
				{
					//Only queen is necessary here later on!
					moves.push_back(Move::getInt16FromPos(i, i + 8, 0, 0));
					moves.push_back(Move::getInt16FromPos(i, i + 8, 1, 0));
					moves.push_back(Move::getInt16FromPos(i, i + 8, 2, 0));
					moves.push_back(Move::getInt16FromPos(i, i + 8, 3, 0));
				}
				else
				{
					moves.push_back(Move::getInt16FromPos(i, i + 8));
					if (i > 7 && i < 16 && board[i + 16] == 0)
					{
						moves.push_back(Move::getInt16FromPos(i, i + 16));
					}
				}
			}
			//Taking pieces
			if (((i + 7) / 8 == (i / 8) + 1) && board[i + 7] != 0 && board[i + 7] < 8)
			{
				if (i > 47) //Promotion straight
				{
					//Only queen is necessary here later on!
					moves.push_back(Move::getInt16FromPos(i, i + 7, 0, 0));
					moves.push_back(Move::getInt16FromPos(i, i + 7, 1, 0));
					moves.push_back(Move::getInt16FromPos(i, i + 7, 2, 0));
					moves.push_back(Move::getInt16FromPos(i, i + 7, 3, 0));
				}
				else
					moves.push_back(Move::getInt16FromPos(i, i + 7));
			}
			if (((i + 9) / 8 == (i / 8) + 1) && board[i + 9] != 0 && board[i + 9] < 8)
			{
				if (i > 47) //Promotion straight
				{
					//Only queen is necessary here later on!
					moves.push_back(Move::getInt16FromPos(i, i + 9, 0, 0));
					moves.push_back(Move::getInt16FromPos(i, i + 9, 1, 0));
					moves.push_back(Move::getInt16FromPos(i, i + 9, 2, 0));
					moves.push_back(Move::getInt16FromPos(i, i + 9, 3, 0));
				}
				else
					moves.push_back(Move::getInt16FromPos(i, i + 9));
			}
			break;
		case 12: //Black rook
			index = i + 8;
			while (index < 64) //+8, outside board check
			{
				if (board[index] != 0)
				{
					if (board[index] < 8) //Can take
					{
						moves.push_back(Move::getInt16FromPos(i, index));
					}
					break; //Otherwise, stop looking
				}
				//Empty square, add move
				moves.push_back(Move::getInt16FromPos(i, index));
				index = index + 8;
			}
			index = i - 8;
			while (index > -1) //-8, outside board check
			{
				if (board[index] != 0)
				{
					if (board[index] < 8) //Can take
					{
						moves.push_back(Move::getInt16FromPos(i, index));
					}
					break; //Otherwise, stop looking
				}
				//Empty square, add move
				moves.push_back(Move::getInt16FromPos(i, index));
				index = index - 8;
			}
			index = i + 1;
			while (index / 8 == i / 8) //+8, outside board check
			{
				if (board[index] != 0)
				{
					if (board[index] < 8) //Can take
					{
						moves.push_back(Move::getInt16FromPos(i, index));
					}
					break; //Otherwise, stop looking
				}
				//Empty square, add move
				moves.push_back(Move::getInt16FromPos(i, index));
				index = index + 1;
			}
			index = i - 1;
			while (index > -1 && index / 8 == i / 8) //+8, outside board check
			{
				if (board[index] != 0)
				{
					if (board[index] < 8) //Can take
					{
						moves.push_back(Move::getInt16FromPos(i, index));
					}
					break; //Otherwise, stop looking
				}
				//Empty square, add move
				moves.push_back(Move::getInt16FromPos(i, index));
				index = index - 1;
			}
			break;
		case 13: //Black knight
				 //Check specific integers and make sure they're on the right line (not outside board)
				 //Eight moves:
				 //Y: +2, X: +1 (16+1 = 17)
				 //Y: -2, X: +1 (-16+1 = -15)
				 //Y: +2, X: -1 (16-1 = 15)
				 //Y: -2, X: -1 (-16-1 = -17)
				 //Y: +1, X: +2 (8+2 = 10)
				 //Y: -1, X: +2 (-8+2 = -6)
				 //Y: +1, X: -2 (8-2 = 6)
				 //Y: -1, X: -2 (-8-2 = -10)
			if (i + 17 < 64 && board[i+17] < 8 //17
				&& ((i + 17) / 8 == i / 8 + 2))
			{
				moves.push_back(Move::getInt16FromPos(i, i + 17));
			}
			if (i > 16 && board[i-17] < 8  //-17
				&& ((i - 17) / 8 == i / 8 - 2))
			{
				moves.push_back(Move::getInt16FromPos(i, i - 17));
			}
			if (i + 15 < 64 && board[i+15] < 8  //15
				&& ((i + 15) / 8 == i / 8 + 2))
			{
				moves.push_back(Move::getInt16FromPos(i, i + 15));
			}
			if (i > 14 && board[i-15] < 8 //-15
				&& ((i - 15) / 8 == i / 8 - 2))
			{
				moves.push_back(Move::getInt16FromPos(i, i - 15));
			}
			if (i + 10 < 64 && board[i+10] < 8  //10
				&& ((i + 10) / 8 == i / 8 + 1))
			{
				moves.push_back(Move::getInt16FromPos(i, i + 10));
			}
			if (i > 9 && board[i-10] < 8  //-10
				&& ((i - 10) / 8 == i / 8 - 1))
			{
				moves.push_back(Move::getInt16FromPos(i, i - 10));
			}
			if (i + 6 < 64 && board[i+6] < 8  //6
				&& ((i + 6) / 8 == i / 8 + 1))
			{
				moves.push_back(Move::getInt16FromPos(i, i + 6));
			}
			if (i > 5 && board[i-6] < 8  //-6
				&& ((i - 6) / 8 == i / 8 - 1))
			{
				moves.push_back(Move::getInt16FromPos(i, i - 6));
			}
			break;
		case 14: //Black bishop
			index = i + 9;
			while (index < 64 && index / 8 == (index - 9) / 8 + 1) //+9, outside board check
			{
				if (board[index] != 0)
				{
					if (board[index] < 8) //Can take
					{
						moves.push_back(Move::getInt16FromPos(i, index));
					}
					break; //Otherwise, stop looking
				}
				//Empty square, add move
				moves.push_back(Move::getInt16FromPos(i, index));
				index = index + 9;
			}
			index = i - 9;
			while (index > -1 && index / 8 == (index + 9) / 8 - 1) //-9, outside board check
			{
				if (board[index] != 0)
				{
					if (board[index] < 8) //Can take
					{
						moves.push_back(Move::getInt16FromPos(i, index));
					}
					break; //Otherwise, stop looking
				}
				//Empty square, add move
				moves.push_back(Move::getInt16FromPos(i, index));
				index = index - 9;
			}
			index = i + 7;
			while (index < 64 && index / 8 == (index - 7) / 8 + 1) //+7, outside board check
			{
				if (board[index] != 0)
				{
					if (board[index] < 8) //Can take
					{
						moves.push_back(Move::getInt16FromPos(i, index));
					}
					break; //Otherwise, stop looking
				}
				//Empty square, add move
				moves.push_back(Move::getInt16FromPos(i, index));
				index = index + 7;
			}
			index = i - 7;
			while (index > -1 && index / 8 == (index + 7) / 8 - 1) //-7, outside board check
			{
				if (board[index] != 0)
				{
					if (board[index] < 8) //Can take
					{
						moves.push_back(Move::getInt16FromPos(i, index));
					}
					break; //Otherwise, stop looking
				}
				//Empty square, add move
				moves.push_back(Move::getInt16FromPos(i, index));
				index = index - 7;
			}
			break;
		case 15: //Black queen
			//Rook part of movement
			index = i + 8;
			while (index < 64) //+8, outside board check
			{
				if (board[index] != 0)
				{
					if (board[index] < 8) //Can take
					{
						moves.push_back(Move::getInt16FromPos(i, index));
					}
					break; //Otherwise, stop looking
				}
				//Empty square, add move
				moves.push_back(Move::getInt16FromPos(i, index));
				index = index + 8;
			}
			index = i - 8;
			while (index > -1) //-8, outside board check
			{
				if (board[index] != 0)
				{
					if (board[index] < 8) //Can take
					{
						moves.push_back(Move::getInt16FromPos(i, index));
					}
					break; //Otherwise, stop looking
				}
				//Empty square, add move
				moves.push_back(Move::getInt16FromPos(i, index));
				index = index - 8;
			}
			index = i + 1;
			while (index / 8 == i / 8) //+8, outside board check
			{
				if (board[index] != 0)
				{
					if (board[index] < 8) //Can take
					{
						moves.push_back(Move::getInt16FromPos(i, index));
					}
					break; //Otherwise, stop looking
				}
				//Empty square, add move
				moves.push_back(Move::getInt16FromPos(i, index));
				index = index + 1;
			}
			index = i - 1;
			while (index > -1 && index / 8 == i / 8) //+8, outside board check
			{
				if (board[index] != 0)
				{
					if (board[index] < 8) //Can take
					{
						moves.push_back(Move::getInt16FromPos(i, index));
					}
					break; //Otherwise, stop looking
				}
				//Empty square, add move
				moves.push_back(Move::getInt16FromPos(i, index));
				index = index - 1;
			}
			//Bishop part
			index = i + 9;
			while (index < 64 && index / 8 == (index - 9) / 8 + 1) //+9, outside board check
			{
				if (board[index] != 0)
				{
					if (board[index] < 8) //Can take
					{
						moves.push_back(Move::getInt16FromPos(i, index));
					}
					break; //Otherwise, stop looking
				}
				//Empty square, add move
				moves.push_back(Move::getInt16FromPos(i, index));
				index = index + 9;
			}
			index = i - 9;
			while (index > -1 && index / 8 == (index + 9) / 8 - 1) //-9, outside board check
			{
				if (board[index] != 0)
				{
					if (board[index] < 8) //Can take
					{
						moves.push_back(Move::getInt16FromPos(i, index));
					}
					break; //Otherwise, stop looking
				}
				//Empty square, add move
				moves.push_back(Move::getInt16FromPos(i, index));
				index = index - 9;
			}
			index = i + 7;
			while (index < 64 && index / 8 == (index - 7) / 8 + 1) //+7, outside board check
			{
				if (board[index] != 0)
				{
					if (board[index] < 8) //Can take
					{
						moves.push_back(Move::getInt16FromPos(i, index));
					}
					break; //Otherwise, stop looking
				}
				//Empty square, add move
				moves.push_back(Move::getInt16FromPos(i, index));
				index = index + 7;
			}
			index = i - 7;
			while (index > -1 && index / 8 == (index + 7) / 8 - 1) //-7, outside board check
			{
				if (board[index] != 0)
				{
					if (board[index] < 8) //Can take
					{
						moves.push_back(Move::getInt16FromPos(i, index));
					}
					break; //Otherwise, stop looking
				}
				//Empty square, add move
				moves.push_back(Move::getInt16FromPos(i, index));
				index = index - 7;
			}
			break;
		case 16: //Black king
			if (i > 0 && board[i - 1] < 8 && (i - 1) / 8 == i / 8) //-1
			{
				if (!squareAttackedByWhite(i - 1))
					moves.push_back(Move::getInt16FromPos(i, i - 1));
			}
			if (i > 8 && board[i - 9] < 8 && (i - 9) / 8 == i / 8 - 1) //-9
			{
				if (!squareAttackedByWhite(i - 9))
					moves.push_back(Move::getInt16FromPos(i, i - 9));
			}
			if (i > 7 && board[i - 8] < 8 && (i - 8) / 8 == i / 8 - 1) //-8
			{
				if (!squareAttackedByWhite(i - 8))
					moves.push_back(Move::getInt16FromPos(i, i - 8));
			}
			if (i > 6 && board[i - 7] < 8 && (i - 7) / 8 == i / 8 - 1) //-7
			{
				if (!squareAttackedByWhite(i - 7))
					moves.push_back(Move::getInt16FromPos(i, i - 7));
			}
			if (i < 63 && board[i + 1] < 8 && (i + 1) / 8 == i / 8) //+1
			{
				if (!squareAttackedByWhite(i + 1))
					moves.push_back(Move::getInt16FromPos(i, i + 1));
			}
			if (i < 55 && board[i + 9] < 8 && (i + 9) / 8 == i / 8 + 1) //+9
			{
				if (!squareAttackedByWhite(i + 9))
					moves.push_back(Move::getInt16FromPos(i, i + 9));
			}
			if (i < 56 && board[i + 8] < 8 && (i + 8) / 8 == i / 8 + 1) //+8
			{
				if (!squareAttackedByWhite(i + 8))
				moves.push_back(Move::getInt16FromPos(i, i + 8));
			}
			if (i < 57 && board[i + 7] < 8 && (i + 7) / 8 == i / 8 + 1) //+7
			{
				if (!squareAttackedByWhite(i + 7))
					moves.push_back(Move::getInt16FromPos(i, i + 7));
			}
			break;
		}
	}
	//En passant (-1 for no en passant)
	if (enPassantSquare > 0) //This means that black can perform en passant
	{
			if (((enPassantSquare - 9) / 8 == (enPassantSquare / 8) - 1) && board[enPassantSquare - 9] == 11)
			{
				//Black pawn can take en passant square
				//Does this put king in check?
				//Expensive check is fine, this happens so rarely
				board[enPassantSquare - 9] = 0;
				board[enPassantSquare - 8] = 0;
				board[enPassantSquare] = 11;

				if (!squareAttackedByWhite(kingIndex))
					moves.push_back(Move::getInt16FromPos(enPassantSquare - 9, enPassantSquare));
				board[enPassantSquare - 9] = 11;
				board[enPassantSquare - 8] = 1;
				board[enPassantSquare] = 0;
			}
			if (((enPassantSquare - 7) / 8 == (enPassantSquare / 8) - 1) && board[enPassantSquare - 7] == 11)
			{
				//Black pawn can take en passant square
				board[enPassantSquare - 7] = 0;
				board[enPassantSquare - 8] = 0;
				board[enPassantSquare] = 11;
				if (!squareAttackedByWhite(kingIndex))
					moves.push_back(Move::getInt16FromPos(enPassantSquare - 7, enPassantSquare));
				board[enPassantSquare - 7] = 11;
				board[enPassantSquare - 8] = 1;
				board[enPassantSquare] = 0;
			}
	}

	if (castlingBlackKingSide)
	{
		if (board[5] == 0 && board[6] == 0) //If the squares are empty
		{
			if (!squareAttackedByWhite(5) && !squareAttackedByWhite(6))
				moves.push_back(Move::getInt16FromPos(4, 6));
		}
	}
	if (castlingBlackQueenSide)
	{
		if (board[3] == 0 && board[2] == 0 && board[1] == 0) //If the squares are empty
		{
			if (!squareAttackedByWhite(3) && !squareAttackedByWhite(2))
			moves.push_back(Move::getInt16FromPos(4, 2));
		}
	}
	return moves;
}

bool ArrayBoard::squareAttackedByBlack(int piecePos)
{
	//Passes through king for kingEvasion reasons
	//Look if square is attacked by black. If it is, return true. Else, false
	//Method: Start with pawn, bishop, king. Use static square check
	//Continue with sliding pieces. Check for alignment, then for direct access.
	//Pawns
	int pieceRow = piecePos / 8;
	if (piecePos - 9 > -1 && board[piecePos - 9] == 11 && diagonalArray[piecePos] == diagonalArray[piecePos - 9])
		return true;
	else if (piecePos - 7 > -1 && board[piecePos - 7] == 11 && antiDiagonalArray[piecePos] == antiDiagonalArray[piecePos - 7])
		return true;
	//Knights
	else if (piecePos + 17 < 64 && (board[piecePos + 17] == 13) //17
		&& ((piecePos + 17) / 8 == pieceRow + 2))
	{
		return true;
	}
	else if (piecePos > 16 && (board[piecePos - 17] == 13) //-17
		&& ((piecePos - 17) / 8 == pieceRow - 2))
	{
		return true;
	}
	else if (piecePos + 15 < 64 && (board[piecePos + 15] == 13) //15
		&& ((piecePos + 15) / 8 == pieceRow + 2))
	{
		return true;
	}
	else if (piecePos > 14 && (board[piecePos - 15] == 13) //-15
		&& ((piecePos - 15) / 8 == pieceRow - 2))
	{
		return true;
	}
	else if (piecePos + 10 < 64 && (board[piecePos + 10] == 13) //10
		&& ((piecePos + 10) / 8 == pieceRow + 1))
	{
		return true;
	}
	else if (piecePos > 9 && (board[piecePos - 10] == 13) //-10
		&& ((piecePos - 10) / 8 == pieceRow - 1))
	{
		return true;
	}
	else if (piecePos + 6 < 64 && (board[piecePos + 6] == 13) //6
		&& ((piecePos + 6) / 8 == pieceRow + 1))
	{
		return true;
	}
	else if (piecePos > 5 && (board[piecePos - 6] == 13) //-6
		&& ((piecePos - 6) / 8 == pieceRow - 1))
	{
		return true;
	}
	int pieceColumn = piecePos % 8;
	int index;
	for (size_t i = 0; i < 64; i++) //Find black sliding pieces
	{
		//Check whether they align with the white king
		switch (board[i])
		{
		case 12: //Rook [FULLY FUNCTIONAL]
			if (i % 8 == pieceColumn) //Same column
			{
				//King could be threatened by this piece. Do raycast
				//Check if king is above or below
				if (i > piecePos) //Black rook moving UP
				{
					index = i - 8;
					//Check move until piece is reached
					while (index != piecePos)
					{
						if (board[index] != 0 && board[index] != 6) //If it encounters something
						{
							break;
						}
						index = index - 8;
					}
					if (index == piecePos)
					{
						//No piece inbetween! The piece is in check!
						return true;
					}
				}
				else //Other direction, black rook moving DOWN
				{
					index = i + 8;
					//Check move until king is reached. There has to be a piece inbetween somewhere
					while (index != piecePos)
					{
						if (board[index] != 0 && board[index] != 6) //If it encounters something
						{
							break;
						}
						index = index + 8;
					}
					if (index == piecePos)
					{
						//No piece inbetween! The piece is in check!
						return true;
					}
				}

			}
			else if (i / 8 == pieceRow) //Same row
			{
				//King could be threatened by this piece. Do raycast
				//Check if king is above or below
				if (i > piecePos) //Black rook moving LEFT
				{
					index = i - 1;
					//Check move until king is reached. There has to be a piece inbetween somewhere
					while (index != piecePos)
					{
						if (board[index] != 0 && board[index] != 6) //If it encounters something
						{
							break;
						}
						index = index - 1;
					}
					if (index == piecePos)
					{
						//No piece inbetween! The piece is in check!
						return true;
					}
				}
				else //Other direction, black rook moving RIGHT
				{
					index = i + 1;
					//Check move until king is reached. There has to be a piece inbetween somewhere
					while (index != piecePos)
					{
						if (board[index] != 0 && board[index] != 6) //If it encounters something
						{
							break;
						}
						index = index + 1;
					}
					if (index == piecePos)
					{
						//No piece inbetween! The piece is in check!
						return true;
					}
				}
			}
			break;
		case 14: //Bishop [FULLY FUNCTIONAL]
			if (diagonalArray[i] == diagonalArray[piecePos]) //Diagonal: main file a1h8
			{
				if (i > piecePos) //Black bishop moving diagonal UP
				{
					index = i - 9;
					//Check move until king is reached. There has to be a piece inbetween somewhere
					while (index != piecePos)
					{
						if (board[index] != 0 && board[index] != 6) //If it encounters something
						{
							break;
						}
						index = index - 9;
					}
					if (index == piecePos)
					{
						//No piece inbetween! The piece is in check!
						return true;
					}
				}
				else //Other direction, black rook moving DOWN
				{
					index = i + 9;
					//Check move until king is reached. There has to be a piece inbetween somewhere
					while (index != piecePos)
					{
						if (board[index] != 0 && board[index] != 6) //If it encounters something
						{
							break;
						}
						index = index + 9;
					}
					if (index == piecePos)
					{
						//No piece inbetween! The piece is in check!
						return true;
					}
				}

			}
			else if (antiDiagonalArray[i] == antiDiagonalArray[piecePos])
			{
				if (i > piecePos) //Black bishop moving diagonal UP
				{
					index = i - 7;
					//Check move until king is reached. There has to be a piece inbetween somewhere
					while (index != piecePos)
					{
						if (board[index] != 0 && board[index] != 6) //If it encounters something
						{
							break;
						}
						index = index - 7;
					}
					if (index == piecePos)
					{
						//No piece inbetween! The piece is in check!
						return true;
					}
				}
				else //Other direction, black rook moving DOWN
				{
					index = i + 7;
					//Check move until king is reached. There has to be a piece inbetween somewhere
					while (index != piecePos)
					{
						if (board[index] != 0 && board[index] != 6) //If it encounters something
						{
							break;
						}
						index = index + 7;
					}
					if (index == piecePos)
					{
						//No piece inbetween! The piece is in check!
						return true;
					}
				}

			}
			break;
		case 15: //Queen [FULLY FUNCTIONAL]
			if (i % 8 == pieceColumn) //Same column
			{
				//King could be threatened by this piece. Do raycast
				//Check if king is above or below
				if (i > piecePos) //Black rook moving UP
				{
					index = i - 8;
					//Check move until piece is reached
					while (index != piecePos)
					{
						if (board[index] != 0 && board[index] != 6) //If it encounters something
						{
							break;
						}
						index = index - 8;
					}
					if (index == piecePos)
					{
						//No piece inbetween! The piece is in check!
						return true;
					}
				}
				else //Other direction, black rook moving DOWN
				{
					index = i + 8;
					//Check move until king is reached. There has to be a piece inbetween somewhere
					while (index != piecePos)
					{
						if (board[index] != 0 && board[index] != 6) //If it encounters something
						{
							break;
						}
						index = index + 8;
					}
					if (index == piecePos)
					{
						//No piece inbetween! The piece is in check!
						return true;
					}
				}

			}
			else if (i / 8 == pieceRow) //Same row
			{
				//King could be threatened by this piece. Do raycast
				//Check if king is above or below
				if (i > piecePos) //Black rook moving LEFT
				{
					index = i - 1;
					//Check move until king is reached. There has to be a piece inbetween somewhere
					while (index != piecePos)
					{
						if (board[index] != 0 && board[index] != 6) //If it encounters something
						{
							break;
						}
						index = index - 1;
					}
					if (index == piecePos)
					{
						//No piece inbetween! The piece is in check!
						return true;
					}
				}
				else //Other direction, black rook moving RIGHT
				{
					index = i + 1;
					//Check move until king is reached. There has to be a piece inbetween somewhere
					while (index != piecePos)
					{
						if (board[index] != 0 && board[index] != 6) //If it encounters something
						{
							break;
						}
						index = index + 1;
					}
					if (index == piecePos)
					{
						//No piece inbetween! The piece is in check!
						return true;
					}
				}
			}
			else if (diagonalArray[i] == diagonalArray[piecePos]) //Diagonal: main file a1h8
			{
				if (i > piecePos) //Black bishop moving diagonal UP
				{
					index = i - 9;
					//Check move until king is reached. There has to be a piece inbetween somewhere
					while (index != piecePos)
					{
						if (board[index] != 0 && board[index] != 6) //If it encounters something
						{
							break;
						}
						index = index - 9;
					}
					if (index == piecePos)
					{
						//No piece inbetween! The piece is in check!
						return true;
					}
				}
				else //Other direction, black rook moving DOWN
				{
					index = i + 9;
					//Check move until king is reached. There has to be a piece inbetween somewhere
					while (index != piecePos)
					{
						if (board[index] != 0 && board[index] != 6) //If it encounters something
						{
							break;
						}
						index = index + 9;
					}
					if (index == piecePos)
					{
						//No piece inbetween! The piece is in check!
						return true;
					}
				}

			}
			else if (antiDiagonalArray[i] == antiDiagonalArray[piecePos])
			{
				if (i > piecePos) //Black bishop moving diagonal UP
				{
					index = i - 7;
					//Check move until king is reached. There has to be a piece inbetween somewhere
					while (index != piecePos)
					{
						if (board[index] != 0 && board[index] != 6) //If it encounters something
						{
							break;
						}
						index = index - 7;
					}
					if (index == piecePos)
					{
						//No piece inbetween! The piece is in check!
						return true;
					}
				}
				else //Other direction, black rook moving DOWN
				{
					index = i + 7;
					//Check move until king is reached. There has to be a piece inbetween somewhere
					while (index != piecePos)
					{
						if (board[index] != 0 && board[index] != 6) //If it encounters something
						{
							break;
						}
						index = index + 7;
					}
					if (index == piecePos)
					{
						//No piece inbetween! The piece is in check!
						return true;
					}
				}

			}
			break;
		}
	}
	if (piecePos > 0 && board[piecePos - 1] == 16 && (piecePos - 1) / 8 == pieceRow) //-1
	{
		return true;
	}
	else if (piecePos > 8 && board[piecePos - 9] == 16 && (piecePos - 9) / 8 == pieceRow - 1) //-9
	{
		return true;
	}
	else if (piecePos > 7 && board[piecePos - 8] == 16 && (piecePos - 8) / 8 == pieceRow - 1) //-8
	{
		return true;
	}
	else if (piecePos > 6 && board[piecePos - 7] == 16 && (piecePos - 7) / 8 == pieceRow - 1) //-7
	{
		return true;
	}
	else if (piecePos < 63 && board[piecePos + 1] == 16 && (piecePos + 1) / 8 == pieceRow) //+1
	{
		return true;
	}
	else if (piecePos < 54 && board[piecePos + 9] == 16 && (piecePos + 9) / 8 == pieceRow + 1) //+9
	{
		return true;
	}
	else if (piecePos < 55 && board[piecePos + 8] == 16 && (piecePos + 8) / 8 == pieceRow + 1) //+8
	{
		return true;
	}
	else if (piecePos < 56 && board[piecePos + 7] == 16 && (piecePos + 7) / 8 == pieceRow + 1) //+7
	{
		return true;
	}
	return false;
}

bool ArrayBoard::squareAttackedByWhite(int piecePos)
{
	//Look if square is attacked by white. If it is, return true. Else, false
	//Method: Start with pawn, bishop, king. Use static square check
	//Continue with sliding pieces. Check for alignment, then for direct access.
	//Pawns
	int pieceRow = piecePos / 8;
	if (piecePos + 9 < 64 && board[piecePos + 9] == 1 && diagonalArray[piecePos] == diagonalArray[piecePos + 9])
		return true;
	else if (piecePos + 7 < 64 && board[piecePos + 7] == 1 && antiDiagonalArray[piecePos] == antiDiagonalArray[piecePos + 7])
		return true;
	//Knights
	else if (piecePos + 17 < 64 && (board[piecePos + 17] == 3) //17
		&& ((piecePos + 17) / 8 == pieceRow + 2))
	{
		return true;
	}
	else if (piecePos > 16 && (board[piecePos - 17] == 3) //-17
		&& ((piecePos - 17) / 8 == pieceRow - 2))
	{
		return true;
	}
	else if (piecePos + 15 < 64 && (board[piecePos + 15] == 3) //15
		&& ((piecePos + 15) / 8 == pieceRow + 2))
	{
		return true;
	}
	else if (piecePos > 14 && (board[piecePos - 15] == 3) //-15
		&& ((piecePos - 15) / 8 == pieceRow - 2))
	{
		return true;
	}
	else if (piecePos + 10 < 64 && (board[piecePos + 10] == 3) //10
		&& ((piecePos + 10) / 8 == pieceRow + 1))
	{
		return true;
	}
	else if (piecePos > 9 && (board[piecePos - 10] == 3) //-10
		&& ((piecePos - 10) / 8 == pieceRow - 1))
	{
		return true;
	}
	else if (piecePos + 6 < 64 && (board[piecePos + 6] == 3) //6
		&& ((piecePos + 6) / 8 == pieceRow + 1))
	{
		return true;
	}
	else if (piecePos > 5 && (board[piecePos - 6] == 3) //-6
		&& ((piecePos - 6) / 8 == pieceRow - 1))
	{
		return true;
	}
	int pieceColumn = piecePos % 8;
	int index;
	for (size_t i = 0; i < 64; i++) //Find black sliding pieces
	{
		//Check whether they align with the white king
		switch (board[i])
		{
		case 2: //Rook [FULLY FUNCTIONAL]
			if (i % 8 == pieceColumn) //Same column
			{
				//King could be threatened by this piece. Do raycast
				//Check if king is above or below
				if (i > piecePos) //Black rook moving UP
				{
					index = i - 8;
					//Check move until piece is reached
					while (index != piecePos)
					{
						if (board[index] != 0 && board[index] != 16) //If it encounters something
						{
							break;
						}
						index = index - 8;
					}
					if (index == piecePos)
					{
						//No piece inbetween! The piece is in check!
						return true;
					}
				}
				else //Other direction, black rook moving DOWN
				{
					index = i + 8;
					//Check move until king is reached. There has to be a piece inbetween somewhere
					while (index != piecePos)
					{
						if (board[index] != 0 && board[index] != 16) //If it encounters something
						{
							break;
						}
						index = index + 8;
					}
					if (index == piecePos)
					{
						//No piece inbetween! The piece is in check!
						return true;
					}
				}

			}
			else if (i / 8 == pieceRow) //Same row
			{
				//King could be threatened by this piece. Do raycast
				//Check if king is above or below
				if (i > piecePos) //Black rook moving LEFT
				{
					index = i - 1;
					//Check move until king is reached. There has to be a piece inbetween somewhere
					while (index != piecePos)
					{
						if (board[index] != 0 && board[index] != 16) //If it encounters something
						{
							break;
						}
						index = index - 1;
					}
					if (index == piecePos)
					{
						//No piece inbetween! The piece is in check!
						return true;
					}
				}
				else //Other direction, black rook moving RIGHT
				{
					index = i + 1;
					//Check move until king is reached. There has to be a piece inbetween somewhere
					while (index != piecePos)
					{
						if (board[index] != 0 && board[index] != 16) //If it encounters something
						{
							break;
						}
						index = index + 1;
					}
					if (index == piecePos)
					{
						//No piece inbetween! The piece is in check!
						return true;
					}
				}
			}
			break;
		case 4: //Bishop [FULLY FUNCTIONAL]
			if (diagonalArray[i] == diagonalArray[piecePos]) //Diagonal: main file a1h8
			{
				if (i > piecePos) //Black bishop moving diagonal UP
				{
					index = i - 9;
					//Check move until king is reached. There has to be a piece inbetween somewhere
					while (index != piecePos)
					{
						if (board[index] != 0 && board[index] != 16) //If it encounters something
						{
							break;
						}
						index = index - 9;
					}
					if (index == piecePos)
					{
						//No piece inbetween! The piece is in check!
						return true;
					}
				}
				else //Other direction, black rook moving DOWN
				{
					index = i + 9;
					//Check move until king is reached. There has to be a piece inbetween somewhere
					while (index != piecePos)
					{
						if (board[index] != 0 && board[index] != 16) //If it encounters something
						{
							break;
						}
						index = index + 9;
					}
					if (index == piecePos)
					{
						//No piece inbetween! The piece is in check!
						return true;
					}
				}

			}
			else if (antiDiagonalArray[i] == antiDiagonalArray[piecePos])
			{
				if (i > piecePos) //Black bishop moving diagonal UP
				{
					index = i - 7;
					//Check move until king is reached. There has to be a piece inbetween somewhere
					while (index != piecePos)
					{
						if (board[index] != 0 && board[index] != 16) //If it encounters something
						{
							break;
						}
						index = index - 7;
					}
					if (index == piecePos)
					{
						//No piece inbetween! The piece is in check!
						return true;
					}
				}
				else //Other direction, black rook moving DOWN
				{
					index = i + 7;
					//Check move until king is reached. There has to be a piece inbetween somewhere
					while (index != piecePos)
					{
						if (board[index] != 0 && board[index] != 16) //If it encounters something
						{
							break;
						}
						index = index + 7;
					}
					if (index == piecePos)
					{
						//No piece inbetween! The piece is in check!
						return true;
					}
				}

			}
			break;
		case 5: //Queen [FULLY FUNCTIONAL]
			if (i % 8 == pieceColumn) //Same column
			{
				//King could be threatened by this piece. Do raycast
				//Check if king is above or below
				if (i > piecePos) //Black rook moving UP
				{
					index = i - 8;
					//Check move until piece is reached
					while (index != piecePos)
					{
						if (board[index] != 0 && board[index] != 16) //If it encounters something
						{
							break;
						}
						index = index - 8;
					}
					if (index == piecePos)
					{
						//No piece inbetween! The piece is in check!
						return true;
					}
				}
				else //Other direction, black rook moving DOWN
				{
					index = i + 8;
					//Check move until king is reached. There has to be a piece inbetween somewhere
					while (index != piecePos)
					{
						if (board[index] != 0 && board[index] != 16) //If it encounters something
						{
							break;
						}
						index = index + 8;
					}
					if (index == piecePos)
					{
						//No piece inbetween! The piece is in check!
						return true;
					}
				}

			}
			else if (i / 8 == pieceRow) //Same row
			{
				//King could be threatened by this piece. Do raycast
				//Check if king is above or below
				if (i > piecePos) //Black rook moving LEFT
				{
					index = i - 1;
					//Check move until king is reached. There has to be a piece inbetween somewhere
					while (index != piecePos)
					{
						if (board[index] != 0 && board[index] != 16) //If it encounters something
						{
							break;
						}
						index = index - 1;
					}
					if (index == piecePos)
					{
						//No piece inbetween! The piece is in check!
						return true;
					}
				}
				else //Other direction, black rook moving RIGHT
				{
					index = i + 1;
					//Check move until king is reached. There has to be a piece inbetween somewhere
					while (index != piecePos)
					{
						if (board[index] != 0 && board[index] != 16) //If it encounters something
						{
							break;
						}
						index = index + 1;
					}
					if (index == piecePos)
					{
						//No piece inbetween! The piece is in check!
						return true;
					}
				}
			}
			else if (diagonalArray[i] == diagonalArray[piecePos]) //Diagonal: main file a1h8
			{
				if (i > piecePos) //Black bishop moving diagonal UP
				{
					index = i - 9;
					//Check move until king is reached. There has to be a piece inbetween somewhere
					while (index != piecePos)
					{
						if (board[index] != 0 && board[index] != 16) //If it encounters something
						{
							break;
						}
						index = index - 9;
					}
					if (index == piecePos)
					{
						//No piece inbetween! The piece is in check!
						return true;
					}
				}
				else //Other direction, black rook moving DOWN
				{
					index = i + 9;
					//Check move until king is reached. There has to be a piece inbetween somewhere
					while (index != piecePos)
					{
						if (board[index] != 0 && board[index] != 16) //If it encounters something
						{
							break;
						}
						index = index + 9;
					}
					if (index == piecePos)
					{
						//No piece inbetween! The piece is in check!
						return true;
					}
				}

			}
			else if (antiDiagonalArray[i] == antiDiagonalArray[piecePos])
			{
				if (i > piecePos) //Black bishop moving diagonal UP
				{
					index = i - 7;
					//Check move until king is reached. There has to be a piece inbetween somewhere
					while (index != piecePos)
					{
						if (board[index] != 0 && board[index] != 16) //If it encounters something
						{
							break;
						}
						index = index - 7;
					}
					if (index == piecePos)
					{
						//No piece inbetween! The piece is in check!
						return true;
					}
				}
				else //Other direction, black rook moving DOWN
				{
					index = i + 7;
					//Check move until king is reached. There has to be a piece inbetween somewhere
					while (index != piecePos)
					{
						if (board[index] != 0 && board[index] != 16) //If it encounters something
						{
							break;
						}
						index = index + 7;
					}
					if (index == piecePos)
					{
						//No piece inbetween! The piece is in check!
						return true;
					}
				}

			}
			break;
		}
	}
	if (piecePos > 0 && board[piecePos - 1] == 6 && (piecePos - 1) / 8 == pieceRow) //-1
	{
		return true;
	}
	else if (piecePos > 8 && board[piecePos - 9] == 6 && (piecePos - 9) / 8 == pieceRow - 1) //-9
	{
		return true;
	}
	else if (piecePos > 7 && board[piecePos - 8] == 6 && (piecePos - 8) / 8 == pieceRow - 1) //-8
	{
		return true;
	}
	else if (piecePos > 6 && board[piecePos - 7] == 6 && (piecePos - 7) / 8 == pieceRow - 1) //-7
	{
		return true;
	}
	else if (piecePos < 63 && board[piecePos + 1] == 6 && (piecePos + 1) / 8 == pieceRow) //+1
	{
		return true;
	}
	else if (piecePos < 54 && board[piecePos + 9] == 6 && (piecePos + 9) / 8 == pieceRow + 1) //+9
	{
		return true;
	}
	else if (piecePos < 55 && board[piecePos + 8] == 6 && (piecePos + 8) / 8 == pieceRow + 1) //+8
	{
		return true;
	}
	else if (piecePos < 56 && board[piecePos + 7] == 6 && (piecePos + 7) / 8 == pieceRow + 1) //+7
	{
		return true;
	}
	return false;
}
 
//Generate king evasion moves. Only king moves can be generated with 2 attacking pieces
vector<__int16> ArrayBoard::generateWhiteEvasionMoves(vector<__int16> &moves, int attackingPos, int piecePos, vector<short> &pinnedPiecesSet)
{
	moves.clear();
	//Only care about sliding pieces for extensive check
	//Find which piece is attacking the king
	//Set attacking pos to the position where the king cannot move
	//Check for blocking moves
	int index = attackingPos;

	int pieceColumn = piecePos % 8;
	int pieceRow = piecePos / 8;


	if (attackingPos > -1) //If there is more than 1 piece attacking king, this is false
	{
		switch (board[attackingPos])
		{
		case 11: //Pawn
			whiteBlockingMoves(moves, attackingPos, pinnedPiecesSet);
		case 12: //Rook [FULLY FUNCTIONAL]
			if (attackingPos % 8 == pieceColumn) //Same column
			{
				if (attackingPos > piecePos) //Black rook moving UP
				{
					//Check move until piece is reached
					while (index != piecePos)
					{
						whiteBlockingMoves(moves, index, pinnedPiecesSet);
						index = index - 8;
					}
				}
				else //Other direction, black rook moving DOWN
				{
					//Check move until king is reached. There has to be a piece inbetween somewhere
					while (index != piecePos)
					{
						whiteBlockingMoves(moves, index, pinnedPiecesSet);
						index = index + 8;
					}
				}

			}
			else if (attackingPos / 8 == pieceRow) //Same row
			{
				//King could be threatened by this piece. Do raycast
				//Check if king is above or below
				if (attackingPos > piecePos) //Black rook moving LEFT
				{
					//Check move until king is reached. There has to be a piece inbetween somewhere
					while (index != piecePos)
					{
						whiteBlockingMoves(moves, index, pinnedPiecesSet);
						index = index - 1;
					}
				}
				else //Other direction, black rook moving RIGHT
				{
					//Check move until king is reached. There has to be a piece inbetween somewhere
					while (index != piecePos)
					{
						whiteBlockingMoves(moves, index, pinnedPiecesSet);
						index = index + 1;
					}
				}
			}
			break;
		case 13: //Bishop
			whiteBlockingMoves(moves, attackingPos, pinnedPiecesSet);
			break;
		case 14: //Bishop [FULLY FUNCTIONAL]
			if (diagonalArray[attackingPos] == diagonalArray[piecePos]) //Diagonal: main file a1h8
			{
				if (attackingPos > piecePos) //Black bishop moving diagonal UP
				{
					//Check move until king is reached. There has to be a piece inbetween somewhere
					while (index != piecePos)
					{
						whiteBlockingMoves(moves, index, pinnedPiecesSet);
						index = index - 9;
					}
				}
				else //Other direction, black rook moving DOWN
				{
					//Check move until king is reached. There has to be a piece inbetween somewhere
					while (index != piecePos)
					{
						whiteBlockingMoves(moves, index, pinnedPiecesSet);
						index = index + 9;
					}
				}

			}
			else if (antiDiagonalArray[attackingPos] == antiDiagonalArray[piecePos])
			{
				if (attackingPos > piecePos) //Black bishop moving diagonal UP
				{
					//Check move until king is reached. There has to be a piece inbetween somewhere
					while (index != piecePos)
					{
						whiteBlockingMoves(moves, index, pinnedPiecesSet);
						index = index - 7;
					}
				}
				else //Other direction, black rook moving DOWN
				{
					//Check move until king is reached. There has to be a piece inbetween somewhere
					while (index != piecePos)
					{
						whiteBlockingMoves(moves, index, pinnedPiecesSet);
						index = index + 7;
					}
				}

			}
			break;
		case 15: //Queen [FULLY FUNCTIONAL]
			if (attackingPos % 8 == pieceColumn) //Same column
			{
				if (attackingPos > piecePos) //Black rook moving UP
				{
					//Check move until piece is reached
					while (index != piecePos)
					{
						whiteBlockingMoves(moves, index, pinnedPiecesSet);
						index = index - 8;
					}
				}
				else //Other direction, black rook moving DOWN
				{
					//Check move until king is reached. There has to be a piece inbetween somewhere
					while (index != piecePos)
					{
						whiteBlockingMoves(moves, index, pinnedPiecesSet);
						index = index + 8;
					}
				}

			}
			else if (attackingPos / 8 == pieceRow) //Same row
			{
				//King could be threatened by this piece. Do raycast
				//Check if king is above or below
				if (attackingPos > piecePos) //Black rook moving LEFT
				{
					//Check move until king is reached. There has to be a piece inbetween somewhere
					while (index != piecePos)
					{
						whiteBlockingMoves(moves, index, pinnedPiecesSet);
						index = index - 1;
					}
				}
				else //Other direction, black rook moving RIGHT
				{
					//Check move until king is reached. There has to be a piece inbetween somewhere
					while (index != piecePos)
					{
						whiteBlockingMoves(moves, index, pinnedPiecesSet);
						index = index + 1;
					}
				}
			}
			else if (diagonalArray[attackingPos] == diagonalArray[piecePos]) //Diagonal: main file a1h8
			{
				if (attackingPos > piecePos) //Black bishop moving diagonal UP
				{
					//Check move until king is reached. There has to be a piece inbetween somewhere
					while (index != piecePos)
					{
						whiteBlockingMoves(moves, index, pinnedPiecesSet);
						index = index - 9;
					}
				}
				else //Other direction, black rook moving DOWN
				{
					//Check move until king is reached. There has to be a piece inbetween somewhere
					while (index != piecePos)
					{
						whiteBlockingMoves(moves, index, pinnedPiecesSet);
						index = index + 9;
					}
				}

			}
			else if (antiDiagonalArray[attackingPos] == antiDiagonalArray[piecePos])
			{
				if (attackingPos > piecePos) //Black bishop moving diagonal UP
				{
					//Check move until king is reached. There has to be a piece inbetween somewhere
					while (index != piecePos)
					{
						whiteBlockingMoves(moves, index, pinnedPiecesSet);
						index = index - 7;
					}
				}
				else //Other direction, black rook moving DOWN
				{
					//Check move until king is reached. There has to be a piece inbetween somewhere
					while (index != piecePos)
					{
						whiteBlockingMoves(moves, index, pinnedPiecesSet);
						index = index + 7;
					}
				}

			}
			break;
		}
		//Go between the attacking sliding piece and the king
		//Do whiteBlockingMoves for those squares and check for pieces which can take the king
	}
	
	//King moves
	if (piecePos - 1 > -1 && (board[piecePos - 1] == 0 || board[piecePos - 1] > 8) && (piecePos - 1) / 8 == piecePos / 8) //-1
	{
		if (!squareAttackedByBlack(piecePos - 1))
			moves.push_back(Move::getInt16FromPos(piecePos, piecePos - 1));
	}
	if (piecePos - 9 > -1 && (board[piecePos - 9] == 0 || board[piecePos - 9] > 8) && (piecePos - 9) / 8 == piecePos / 8 - 1) //-9
	{
		if (!squareAttackedByBlack(piecePos - 9))
			moves.push_back(Move::getInt16FromPos(piecePos, piecePos - 9));
	}
	if (piecePos - 8 > -1 && (board[piecePos - 8] == 0 || board[piecePos - 8] > 8) && (piecePos - 8) / 8 == piecePos / 8 - 1) //-8
	{
		if (!squareAttackedByBlack(piecePos - 8))
			moves.push_back(Move::getInt16FromPos(piecePos, piecePos - 8));
	}
	if (piecePos - 7 > -1 && (board[piecePos - 7] == 0 || board[piecePos - 7] > 8) && (piecePos - 7) / 8 == piecePos / 8 - 1) //-7
	{
		if (!squareAttackedByBlack(piecePos - 7))
			moves.push_back(Move::getInt16FromPos(piecePos, piecePos - 7));
	}
	if (piecePos + 1 < 64 && (board[piecePos + 1] == 0 || board[piecePos + 1] > 8) && (piecePos + 1) / 8 == piecePos / 8) //+1
	{
		if (!squareAttackedByBlack(piecePos + 1))
			moves.push_back(Move::getInt16FromPos(piecePos, piecePos + 1));
	}
	if (piecePos + 9 < 64 && (board[piecePos + 9] == 0 || board[piecePos + 9] > 8) && (piecePos + 9) / 8 == piecePos / 8 + 1) //+9
	{
		if (!squareAttackedByBlack(piecePos + 9))
			moves.push_back(Move::getInt16FromPos(piecePos, piecePos + 9));
	}
	if (piecePos + 8 < 64 && (board[piecePos + 8] == 0 || board[piecePos + 8] > 8) && (piecePos + 8) / 8 == piecePos / 8 + 1) //+8
	{
		if (!squareAttackedByBlack(piecePos + 8))
			moves.push_back(Move::getInt16FromPos(piecePos, piecePos + 8));
	}
	if (piecePos + 7 < 64 && (board[piecePos + 7] == 0 || board[piecePos + 7] > 8) && (piecePos + 7) / 8 == piecePos / 8 + 1) //+7
	{
		if (!squareAttackedByBlack(piecePos + 7))
			moves.push_back(Move::getInt16FromPos(piecePos, piecePos + 7));
	}

	return moves;
	//Check for king moves
}

vector<__int16> ArrayBoard::generateBlackEvasionMoves(vector<__int16> &moves, int attackingPos, int piecePos, vector<short> &pinnedPiecesSet)
{
	moves.clear();
	int index = attackingPos;

	int pieceColumn = piecePos % 8;
	int pieceRow = piecePos / 8;

	if (attackingPos > -1) //If there is more than 1 piece attacking king, this is false
	{
		switch (board[attackingPos])
		{
		case 1: //Pawn
			blackBlockingMoves(moves, attackingPos, pinnedPiecesSet);
		case 2: //Rook [FULLY FUNCTIONAL]
			if (attackingPos % 8 == pieceColumn) //Same column
			{
				if (attackingPos > piecePos) //Black rook moving UP
				{
					//Check move until piece is reached
					while (index != piecePos)
					{
						blackBlockingMoves(moves, index, pinnedPiecesSet);
						index = index - 8;
					}
				}
				else //Other direction, black rook moving DOWN
				{
					//Check move until king is reached. There has to be a piece inbetween somewhere
					while (index != piecePos)
					{
						blackBlockingMoves(moves, index, pinnedPiecesSet);
						index = index + 8;
					}
				}

			}
			else if (attackingPos / 8 == pieceRow) //Same row
			{
				//King could be threatened by this piece. Do raycast
				//Check if king is above or below
				if (attackingPos > piecePos) //Black rook moving LEFT
				{
					//Check move until king is reached. There has to be a piece inbetween somewhere
					while (index != piecePos)
					{
						blackBlockingMoves(moves, index, pinnedPiecesSet);
						index = index - 1;
					}
				}
				else //Other direction, black rook moving RIGHT
				{
					//Check move until king is reached. There has to be a piece inbetween somewhere
					while (index != piecePos)
					{
						blackBlockingMoves(moves, index, pinnedPiecesSet);
						index = index + 1;
					}
				}
			}
			break;
		case 3: //Bishop
			blackBlockingMoves(moves, attackingPos, pinnedPiecesSet);
			break;
		case 4: //Bishop [FULLY FUNCTIONAL]
			if (diagonalArray[attackingPos] == diagonalArray[piecePos]) //Diagonal: main file a1h8
			{
				if (attackingPos > piecePos) //Black bishop moving diagonal UP
				{
					//Check move until king is reached. There has to be a piece inbetween somewhere
					while (index != piecePos)
					{
						blackBlockingMoves(moves, index, pinnedPiecesSet);
						index = index - 9;
					}
				}
				else //Other direction, black rook moving DOWN
				{
					//Check move until king is reached. There has to be a piece inbetween somewhere
					while (index != piecePos)
					{
						blackBlockingMoves(moves, index, pinnedPiecesSet);
						index = index + 9;
					}
				}

			}
			else if (antiDiagonalArray[attackingPos] == antiDiagonalArray[piecePos])
			{
				if (attackingPos > piecePos) //Black bishop moving diagonal UP
				{
					//Check move until king is reached. There has to be a piece inbetween somewhere
					while (index != piecePos)
					{
						blackBlockingMoves(moves, index, pinnedPiecesSet);
						index = index - 7;
					}
				}
				else //Other direction, black rook moving DOWN
				{
					//Check move until king is reached. There has to be a piece inbetween somewhere
					while (index != piecePos)
					{
						blackBlockingMoves(moves, index, pinnedPiecesSet);
						index = index + 7;
					}
				}

			}
			break;
		case 5: //Queen [FULLY FUNCTIONAL]
			if (attackingPos % 8 == pieceColumn) //Same column
			{
				if (attackingPos > piecePos) //Black rook moving UP
				{
					//Check move until piece is reached
					while (index != piecePos)
					{
						blackBlockingMoves(moves, index, pinnedPiecesSet);
						index = index - 8;
					}
				}
				else //Other direction, black rook moving DOWN
				{
					//Check move until king is reached. There has to be a piece inbetween somewhere
					while (index != piecePos)
					{
						blackBlockingMoves(moves, index, pinnedPiecesSet);
						index = index + 8;
					}
				}

			}
			else if (attackingPos / 8 == pieceRow) //Same row
			{
				//King could be threatened by this piece. Do raycast
				//Check if king is above or below
				if (attackingPos > piecePos) //Black rook moving LEFT
				{
					//Check move until king is reached. There has to be a piece inbetween somewhere
					while (index != piecePos)
					{
						blackBlockingMoves(moves, index, pinnedPiecesSet);
						index = index - 1;
					}
				}
				else //Other direction, black rook moving RIGHT
				{
					//Check move until king is reached. There has to be a piece inbetween somewhere
					while (index != piecePos)
					{
						blackBlockingMoves(moves, index, pinnedPiecesSet);
						index = index + 1;
					}
				}
			}
			else if (diagonalArray[attackingPos] == diagonalArray[piecePos]) //Diagonal: main file a1h8
			{
				if (attackingPos > piecePos) //Black bishop moving diagonal UP
				{
					//Check move until king is reached. There has to be a piece inbetween somewhere
					while (index != piecePos)
					{
						blackBlockingMoves(moves, index, pinnedPiecesSet);
						index = index - 9;
					}
				}
				else //Other direction, black rook moving DOWN
				{
					//Check move until king is reached. There has to be a piece inbetween somewhere
					while (index != piecePos)
					{
						blackBlockingMoves(moves, index, pinnedPiecesSet);
						index = index + 9;
					}
				}

			}
			else if (antiDiagonalArray[attackingPos] == antiDiagonalArray[piecePos])
			{
				if (attackingPos > piecePos) //Black bishop moving diagonal UP
				{
					//Check move until king is reached. There has to be a piece inbetween somewhere
					while (index != piecePos)
					{
						blackBlockingMoves(moves, index, pinnedPiecesSet);
						index = index - 7;
					}
				}
				else //Other direction, black rook moving DOWN
				{
					//Check move until king is reached. There has to be a piece inbetween somewhere
					while (index != piecePos)
					{
						blackBlockingMoves(moves, index, pinnedPiecesSet);
						index = index + 7;
					}
				}

			}
			break;
		}
	}

	//King moves
	if (piecePos - 1 > -1 && board[piecePos - 1] < 8 && (piecePos - 1) / 8 == piecePos / 8) //-1
	{
		if (!squareAttackedByWhite(piecePos - 1))
			moves.push_back(Move::getInt16FromPos(piecePos, piecePos - 1));
	}
	if (piecePos - 9 > -1 && board[piecePos - 9] < 8 && (piecePos - 9) / 8 == piecePos / 8 - 1) //-9
	{
		if (!squareAttackedByWhite(piecePos - 9))
			moves.push_back(Move::getInt16FromPos(piecePos, piecePos - 9));
	}
	if (piecePos - 8 > -1 && board[piecePos - 8] < 8 && (piecePos - 8) / 8 == piecePos / 8 - 1) //-8
	{
		if (!squareAttackedByWhite(piecePos - 8))
			moves.push_back(Move::getInt16FromPos(piecePos, piecePos - 8));
	}
	if (piecePos - 7 > -1 && board[piecePos - 7] < 8 && (piecePos - 7) / 8 == piecePos / 8 - 1) //-7
	{
		if (!squareAttackedByWhite(piecePos - 7))
			moves.push_back(Move::getInt16FromPos(piecePos, piecePos - 7));
	}
	if (piecePos + 1 < 64 && board[piecePos + 1] < 8 && (piecePos + 1) / 8 == piecePos / 8) //+1
	{
		if (!squareAttackedByWhite(piecePos + 1))
			moves.push_back(Move::getInt16FromPos(piecePos, piecePos + 1));
	}
	if (piecePos + 9 < 64 && board[piecePos + 9] < 8 && (piecePos + 9) / 8 == piecePos / 8 + 1) //+9
	{
		if (!squareAttackedByWhite(piecePos + 9))
			moves.push_back(Move::getInt16FromPos(piecePos, piecePos + 9));
	}
	if (piecePos + 8 < 64 && board[piecePos + 8] < 8 && (piecePos + 8) / 8 == piecePos / 8 + 1) //+8
	{
		if (!squareAttackedByWhite(piecePos + 8))
			moves.push_back(Move::getInt16FromPos(piecePos, piecePos + 8));
	}
	if (piecePos + 7 < 64 && board[piecePos + 7] < 8 && (piecePos + 7) / 8 == piecePos / 8 + 1) //+7
	{
		if (!squareAttackedByWhite(piecePos + 7))
			moves.push_back(Move::getInt16FromPos(piecePos, piecePos + 7));
	}
	return moves;
}

//Attacksquare, but adds the moves instead of returning true. Also includes pawn moves
void ArrayBoard::whiteBlockingMoves(vector<__int16> &moves, int piecePos, vector<short> &pinnedPiecesSet)
{
	int pieceRow = piecePos / 8;
	bool squareEmpty = board[piecePos] == 0 ? true : false;
	//Pawns
	if (squareEmpty)
	{
		if (pieceRow + 2 == 6 && board[piecePos + 16] == 1 && board[piecePos + 8] == 0 && !(find(pinnedPiecesSet.begin(), pinnedPiecesSet.end(), piecePos + 16) != pinnedPiecesSet.end()))
		{
			moves.push_back(Move::getInt16FromPos(piecePos + 16, piecePos));
		}
		else if (piecePos + 8 < 64 && board[piecePos + 8] == 1 && !(find(pinnedPiecesSet.begin(), pinnedPiecesSet.end(), piecePos + 8) != pinnedPiecesSet.end()))
		{
			if (piecePos < 8) //Promotion straight
			{
				//Only queen is necessary here later on!
				moves.push_back(Move::getInt16FromPos(piecePos + 8, piecePos, 0, 0));
				moves.push_back(Move::getInt16FromPos(piecePos + 8, piecePos, 1, 0));
				moves.push_back(Move::getInt16FromPos(piecePos + 8, piecePos, 2, 0));
				moves.push_back(Move::getInt16FromPos(piecePos + 8, piecePos, 3, 0));
			}
			else
				moves.push_back(Move::getInt16FromPos(piecePos + 8, piecePos));
		}
	}
	else
	{
		//En passant. This is incredibly rare
		if (board[piecePos] == 11 && (int)enPassantSquare == piecePos - 8)
		{
			if ((enPassantSquare + 9) / 8 == (enPassantSquare / 8) + 1 && board[enPassantSquare + 9] == 1)
			{
				int kingIndex;
				for (size_t i = 0; i < 64; i++)
				{
					if (board[i] == 6)
					{
						kingIndex = i;
						break;
					}
				}

				board[enPassantSquare + 9] = 0;
				board[enPassantSquare + 8] = 0;
				board[enPassantSquare] = 1;

				if (!squareAttackedByBlack(kingIndex))
					moves.push_back(Move::getInt16FromPos(enPassantSquare + 9, enPassantSquare));
				board[enPassantSquare + 9] = 1;
				board[enPassantSquare + 8] = 11;
				board[enPassantSquare] = 0;

			}
			if (((enPassantSquare + 7) / 8 == (enPassantSquare / 8) + 1) && board[enPassantSquare + 7] == 1)
			{
				//White pawn can take en passant square while in check
				//This is so rare, looking for king is fine
				int kingIndex;
				for (size_t i = 0; i < 64; i++)
				{
					if (board[i] == 6)
					{
						kingIndex = i;
						break;
					}
				}
				
				board[enPassantSquare + 7] = 0;
				board[enPassantSquare + 8] = 0;
				board[enPassantSquare] = 1;

				if (!squareAttackedByBlack(kingIndex))
					moves.push_back(Move::getInt16FromPos(enPassantSquare + 7, enPassantSquare));
				board[enPassantSquare + 7] = 1;
				board[enPassantSquare + 8] = 11;
				board[enPassantSquare] = 0;
			}
		}

		if (piecePos + 9 < 64 && board[piecePos + 9] == 1 && !squareEmpty && diagonalArray[piecePos] == diagonalArray[piecePos + 9] && !(find(pinnedPiecesSet.begin(), pinnedPiecesSet.end(), piecePos + 9) != pinnedPiecesSet.end()))
		{
			if (piecePos < 8) //Promotion straight
			{
				//Only queen is necessary here later on!
				moves.push_back(Move::getInt16FromPos(piecePos + 9, piecePos, 0, 0));
				moves.push_back(Move::getInt16FromPos(piecePos + 9, piecePos, 1, 0));
				moves.push_back(Move::getInt16FromPos(piecePos + 9, piecePos, 2, 0));
				moves.push_back(Move::getInt16FromPos(piecePos + 9, piecePos, 3, 0));
			}
			else
				moves.push_back(Move::getInt16FromPos(piecePos + 9, piecePos));
		}
		if (board[piecePos + 7] == 1 && !squareEmpty && antiDiagonalArray[piecePos] == antiDiagonalArray[piecePos + 7] && !(find(pinnedPiecesSet.begin(), pinnedPiecesSet.end(), piecePos + 7) != pinnedPiecesSet.end()))
		{
			if (piecePos < 8) //Promotion straight
			{
				//Only queen is necessary here later on!
				moves.push_back(Move::getInt16FromPos(piecePos + 7, piecePos, 0, 0));
				moves.push_back(Move::getInt16FromPos(piecePos + 7, piecePos, 1, 0));
				moves.push_back(Move::getInt16FromPos(piecePos + 7, piecePos, 2, 0));
				moves.push_back(Move::getInt16FromPos(piecePos + 7, piecePos, 3, 0));
			}
			else
				moves.push_back(Move::getInt16FromPos(piecePos + 7, piecePos));
		}
	}
	//Knights
	if (piecePos + 17 < 64 && (board[piecePos + 17] == 3 && !(find(pinnedPiecesSet.begin(), pinnedPiecesSet.end(), piecePos + 17) != pinnedPiecesSet.end())) //17
		&& ((piecePos + 17) / 8 == pieceRow + 2))
	{
		moves.push_back(Move::getInt16FromPos(piecePos + 17, piecePos));
	}
	if (piecePos > 16 && (board[piecePos - 17] == 3) && !(find(pinnedPiecesSet.begin(), pinnedPiecesSet.end(), piecePos - 17) != pinnedPiecesSet.end()) //-17
		&& ((piecePos - 17) / 8 == pieceRow - 2))
	{
		moves.push_back(Move::getInt16FromPos(piecePos - 17, piecePos));
	}
	if (piecePos + 15 < 64 && (board[piecePos + 15] == 3) && !(find(pinnedPiecesSet.begin(), pinnedPiecesSet.end(), piecePos + 15) != pinnedPiecesSet.end()) //15
		&& ((piecePos + 15) / 8 == pieceRow + 2))
	{
		moves.push_back(Move::getInt16FromPos(piecePos + 15, piecePos));
	}
	if (piecePos > 14 && (board[piecePos - 15] == 3) && !(find(pinnedPiecesSet.begin(), pinnedPiecesSet.end(), piecePos - 15) != pinnedPiecesSet.end()) //-15
		&& ((piecePos - 15) / 8 == pieceRow - 2))
	{
		moves.push_back(Move::getInt16FromPos(piecePos - 15, piecePos));
	}
	if (piecePos + 10 < 64 && (board[piecePos + 10] == 3) && !(find(pinnedPiecesSet.begin(), pinnedPiecesSet.end(), piecePos + 10) != pinnedPiecesSet.end()) //10
		&& ((piecePos + 10) / 8 == pieceRow + 1))
	{
		moves.push_back(Move::getInt16FromPos(piecePos + 10, piecePos));
	}
	if (piecePos > 9 && (board[piecePos - 10] == 3) && !(find(pinnedPiecesSet.begin(), pinnedPiecesSet.end(), piecePos - 10) != pinnedPiecesSet.end()) //-10
		&& ((piecePos - 10) / 8 == pieceRow - 1))
	{
		moves.push_back(Move::getInt16FromPos(piecePos - 10, piecePos));
	}
	if (piecePos + 6 < 64 && (board[piecePos + 6] == 3) && !(find(pinnedPiecesSet.begin(), pinnedPiecesSet.end(), piecePos + 6) != pinnedPiecesSet.end()) //6
		&& ((piecePos + 6) / 8 == pieceRow + 1))
	{
		moves.push_back(Move::getInt16FromPos(piecePos + 6, piecePos));
	}
	if (piecePos > 5 && (board[piecePos - 6] == 3) && !(find(pinnedPiecesSet.begin(), pinnedPiecesSet.end(), piecePos - 6) != pinnedPiecesSet.end()) //-6
		&& ((piecePos - 6) / 8 == pieceRow - 1))
	{
		moves.push_back(Move::getInt16FromPos(piecePos - 6, piecePos));
	}
	int pieceColumn = piecePos % 8;
	int index;
	for (size_t i = 0; i < 64; i++) //Find white sliding pieces
	{
		if (find(pinnedPiecesSet.begin(), pinnedPiecesSet.end(), i) != pinnedPiecesSet.end())
			continue;
		//Check whether they align with the white king
		switch (board[i])
		{
		case 2: //Rook [FULLY FUNCTIONAL]
			if (i % 8 == pieceColumn) //Same column
			{
				//King could be threatened by this piece. Do raycast
				//Check if king is above or below
				if (i > piecePos) //Black rook moving UP
				{
					index = i - 8;
					//Check move until piece is reached
					while (index != piecePos)
					{
						if (board[index] != 0) //If it encounters something
						{
							break;
						}
						index = index - 8;
					}
					if (index == piecePos)
					{
						moves.push_back(Move::getInt16FromPos(i, piecePos));
					}
				}
				else //Other direction, black rook moving DOWN
				{
					index = i + 8;
					//Check move until king is reached. There has to be a piece inbetween somewhere
					while (index != piecePos)
					{
						if (board[index] != 0) //If it encounters something
						{
							break;
						}
						index = index + 8;
					}
					if (index == piecePos)
					{
						//No piece inbetween! The piece is in check!
						moves.push_back(Move::getInt16FromPos(i, piecePos));
					}
				}

			}
			else if (i / 8 == pieceRow) //Same row
			{
				//King could be threatened by this piece. Do raycast
				//Check if king is above or below
				if (i > piecePos) //Black rook moving LEFT
				{
					index = i - 1;
					//Check move until king is reached. There has to be a piece inbetween somewhere
					while (index != piecePos)
					{
						if (board[index] != 0) //If it encounters something
						{
							break;
						}
						index = index - 1;
					}
					if (index == piecePos)
					{
						//No piece inbetween! The piece is in check!
						moves.push_back(Move::getInt16FromPos(i, piecePos));
					}
				}
				else //Other direction, black rook moving RIGHT
				{
					index = i + 1;
					//Check move until king is reached. There has to be a piece inbetween somewhere
					while (index != piecePos)
					{
						if (board[index] != 0) //If it encounters something
						{
							break;
						}
						index = index + 1;
					}
					if (index == piecePos)
					{
						//No piece inbetween! The piece is in check!
						moves.push_back(Move::getInt16FromPos(i, piecePos));
					}
				}
			}
			break;
		case 4: //Bishop [FULLY FUNCTIONAL]
			if (diagonalArray[i] == diagonalArray[piecePos]) //Diagonal: main file a1h8
			{
				if (i > piecePos) //Black bishop moving diagonal UP
				{
					index = i - 9;
					//Check move until king is reached. There has to be a piece inbetween somewhere
					while (index != piecePos)
					{
						if (board[index] != 0) //If it encounters something
						{
							break;
						}
						index = index - 9;
					}
					if (index == piecePos)
					{
						//No piece inbetween! The piece is in check!
						moves.push_back(Move::getInt16FromPos(i, piecePos));
					}
				}
				else //Other direction, black rook moving DOWN
				{
					index = i + 9;
					//Check move until king is reached. There has to be a piece inbetween somewhere
					while (index != piecePos)
					{
						if (board[index] != 0) //If it encounters something
						{
							break;
						}
						index = index + 9;
					}
					if (index == piecePos)
					{
						//No piece inbetween! The piece is in check!
						moves.push_back(Move::getInt16FromPos(i, piecePos));
					}
				}

			}
			else if (antiDiagonalArray[i] == antiDiagonalArray[piecePos])
			{
				if (i > piecePos) //Black bishop moving diagonal UP
				{
					index = i - 7;
					//Check move until king is reached. There has to be a piece inbetween somewhere
					while (index != piecePos)
					{
						if (board[index] != 0) //If it encounters something
						{
							break;
						}
						index = index - 7;
					}
					if (index == piecePos)
					{
						//No piece inbetween! The piece is in check!
						moves.push_back(Move::getInt16FromPos(i, piecePos));
					}
				}
				else //Other direction, black rook moving DOWN
				{
					index = i + 7;
					//Check move until king is reached. There has to be a piece inbetween somewhere
					while (index != piecePos)
					{
						if (board[index] != 0) //If it encounters something
						{
							break;
						}
						index = index + 7;
					}
					if (index == piecePos)
					{
						//No piece inbetween! The piece is in check!
						moves.push_back(Move::getInt16FromPos(i, piecePos));
					}
				}

			}
			break;
		case 5: //Queen [FULLY FUNCTIONAL]
			if (i % 8 == pieceColumn) //Same column
			{
				//King could be threatened by this piece. Do raycast
				//Check if king is above or below
				if (i > piecePos) //Black rook moving UP
				{
					index = i - 8;
					//Check move until piece is reached
					while (index != piecePos)
					{
						if (board[index] != 0) //If it encounters something
						{
							break;
						}
						index = index - 8;
					}
					if (index == piecePos)
					{
						//No piece inbetween! The piece is in check!
						moves.push_back(Move::getInt16FromPos(i, piecePos));
					}
				}
				else //Other direction, black rook moving DOWN
				{
					index = i + 8;
					//Check move until king is reached. There has to be a piece inbetween somewhere
					while (index != piecePos)
					{
						if (board[index] != 0) //If it encounters something
						{
							break;
						}
						index = index + 8;
					}
					if (index == piecePos)
					{
						//No piece inbetween! The piece is in check!
						moves.push_back(Move::getInt16FromPos(i, piecePos));
					}
				}

			}
			else if (i / 8 == pieceRow) //Same row
			{
				//King could be threatened by this piece. Do raycast
				//Check if king is above or below
				if (i > piecePos) //Black rook moving LEFT
				{
					index = i - 1;
					//Check move until king is reached. There has to be a piece inbetween somewhere
					while (index != piecePos)
					{
						if (board[index] != 0) //If it encounters something
						{
							break;
						}
						index = index - 1;
					}
					if (index == piecePos)
					{
						//No piece inbetween! The piece is in check!
						moves.push_back(Move::getInt16FromPos(i, piecePos));
					}
				}
				else //Other direction, black rook moving RIGHT
				{
					index = i + 1;
					//Check move until king is reached. There has to be a piece inbetween somewhere
					while (index != piecePos)
					{
						if (board[index] != 0) //If it encounters something
						{
							break;
						}
						index = index + 1;
					}
					if (index == piecePos)
					{
						//No piece inbetween! The piece is in check!
						moves.push_back(Move::getInt16FromPos(i, piecePos));
					}
				}
			}
			else if (diagonalArray[i] == diagonalArray[piecePos]) //Diagonal: main file a1h8
			{
				if (i > piecePos) //Black bishop moving diagonal UP
				{
					index = i - 9;
					//Check move until king is reached. There has to be a piece inbetween somewhere
					while (index != piecePos)
					{
						if (board[index] != 0) //If it encounters something
						{
							break;
						}
						index = index - 9;
					}
					if (index == piecePos)
					{
						//No piece inbetween! The piece is in check!
						moves.push_back(Move::getInt16FromPos(i, piecePos));
					}
				}
				else //Other direction, black rook moving DOWN
				{
					index = i + 9;
					//Check move until king is reached. There has to be a piece inbetween somewhere
					while (index != piecePos)
					{
						if (board[index] != 0) //If it encounters something
						{
							break;
						}
						index = index + 9;
					}
					if (index == piecePos)
					{
						//No piece inbetween! The piece is in check!
						moves.push_back(Move::getInt16FromPos(i, piecePos));
					}
				}

			}
			else if (antiDiagonalArray[i] == antiDiagonalArray[piecePos])
			{
				if (i > piecePos) //Black bishop moving diagonal UP
				{
					index = i - 7;
					//Check move until king is reached. There has to be a piece inbetween somewhere
					while (index != piecePos)
					{
						if (board[index] != 0) //If it encounters something
						{
							break;
						}
						index = index - 7;
					}
					if (index == piecePos)
					{
						//No piece inbetween! The piece is in check!
						moves.push_back(Move::getInt16FromPos(i, piecePos));
					}
				}
				else //Other direction, black rook moving DOWN
				{
					index = i + 7;
					//Check move until king is reached. There has to be a piece inbetween somewhere
					while (index != piecePos)
					{
						if (board[index] != 0) //If it encounters something
						{
							break;
						}
						index = index + 7;
					}
					if (index == piecePos)
					{
						//No piece inbetween! The piece is in check!
						moves.push_back(Move::getInt16FromPos(i, piecePos));
					}
				}

			}
			break;
		}
	}
}

//Attacksquare, but adds the moves instead of returning true
void ArrayBoard::blackBlockingMoves(vector<__int16> &moves, int piecePos, vector<short> &pinnedPiecesSet)
{
	int pieceRow = piecePos / 8;
	bool squareEmpty = board[piecePos] == 0;
	//Pawns
	if (squareEmpty)
	{
		if (pieceRow - 2 == 1 && board[piecePos - 16] == 11 && board[piecePos - 8] == 0 && !(find(pinnedPiecesSet.begin(), pinnedPiecesSet.end(), piecePos - 16) != pinnedPiecesSet.end()))
		{
			moves.push_back(Move::getInt16FromPos(piecePos - 16, piecePos));
		}
		else if (piecePos - 8 > -1 && board[piecePos - 8] == 11 && !(find(pinnedPiecesSet.begin(), pinnedPiecesSet.end(), piecePos - 8) != pinnedPiecesSet.end()))
		{
			if (piecePos > 55) //Promotion straight
			{
				//Only queen is necessary here later on!
				moves.push_back(Move::getInt16FromPos(piecePos - 8, piecePos, 0, 0));
				moves.push_back(Move::getInt16FromPos(piecePos - 8, piecePos, 1, 0));
				moves.push_back(Move::getInt16FromPos(piecePos - 8, piecePos, 2, 0));
				moves.push_back(Move::getInt16FromPos(piecePos - 8, piecePos, 3, 0));
			}
			else
				moves.push_back(Move::getInt16FromPos(piecePos - 8, piecePos));
		}
	}
	else
	{
		if (board[piecePos] == 1 && (int)enPassantSquare == piecePos + 8)
		{
			if ((enPassantSquare - 9) / 8 == (enPassantSquare / 8) - 1 && board[enPassantSquare - 9] == 11)
			{
				int kingIndex;
				for (size_t i = 0; i < 64; i++)
				{
					if (board[i] == 16)
					{
						kingIndex = i;
						break;
					}
				}

				board[enPassantSquare - 9] = 0;
				board[enPassantSquare - 8] = 0;
				board[enPassantSquare] = 11;

				if (!squareAttackedByWhite(kingIndex))
					moves.push_back(Move::getInt16FromPos(enPassantSquare - 9, enPassantSquare));
				board[enPassantSquare - 9] = 11;
				board[enPassantSquare - 8] = 1;
				board[enPassantSquare] = 0;

			}
			if (((enPassantSquare - 7) / 8 == (enPassantSquare / 8) - 1) && board[enPassantSquare - 7] == 11)
			{
				//White pawn can take en passant square while in check
				//This is so rare, looking for king is fine
				int kingIndex;
				for (size_t i = 0; i < 64; i++)
				{
					if (board[i] == 16)
					{
						kingIndex = i;
						break;
					}
				}

				board[enPassantSquare - 7] = 0;
				board[enPassantSquare - 8] = 0;
				board[enPassantSquare] = 11;

				if (!squareAttackedByWhite(kingIndex))
					moves.push_back(Move::getInt16FromPos(enPassantSquare - 7, enPassantSquare));
				board[enPassantSquare - 7] = 11;
				board[enPassantSquare - 8] = 1;
				board[enPassantSquare] = 0;
			}
		}

		if (piecePos - 9 >= 0 && !squareEmpty && board[piecePos - 9] == 11 && diagonalArray[piecePos] == diagonalArray[piecePos - 9] && !(find(pinnedPiecesSet.begin(), pinnedPiecesSet.end(), piecePos - 9) != pinnedPiecesSet.end()))
		{
			if (piecePos > 55) //Promotion straight
			{
				//Only queen is necessary here later on!
				moves.push_back(Move::getInt16FromPos(piecePos - 9, piecePos, 0, 0));
				moves.push_back(Move::getInt16FromPos(piecePos - 9, piecePos, 1, 0));
				moves.push_back(Move::getInt16FromPos(piecePos - 9, piecePos, 2, 0));
				moves.push_back(Move::getInt16FromPos(piecePos - 9, piecePos, 3, 0));
			}
			else
				moves.push_back(Move::getInt16FromPos(piecePos - 9, piecePos));
		}
		if (board[piecePos - 7] == 11 && !squareEmpty && antiDiagonalArray[piecePos] == antiDiagonalArray[piecePos - 7] && !(find(pinnedPiecesSet.begin(), pinnedPiecesSet.end(), piecePos - 7) != pinnedPiecesSet.end()))
		{
			if (piecePos > 55) //Promotion straight
			{
				//Only queen is necessary here later on!
				moves.push_back(Move::getInt16FromPos(piecePos - 7, piecePos, 0, 0));
				moves.push_back(Move::getInt16FromPos(piecePos - 7, piecePos, 1, 0));
				moves.push_back(Move::getInt16FromPos(piecePos - 7, piecePos, 2, 0));
				moves.push_back(Move::getInt16FromPos(piecePos - 7, piecePos, 3, 0));
			}
			else
				moves.push_back(Move::getInt16FromPos(piecePos - 7, piecePos));
		}
	}
	//Knights
	if (piecePos + 17 < 64 && (board[piecePos + 17] == 13) && !(find(pinnedPiecesSet.begin(), pinnedPiecesSet.end(), piecePos + 17) != pinnedPiecesSet.end()) //17
		&& ((piecePos + 17) / 8 == pieceRow + 2))
	{
		moves.push_back(Move::getInt16FromPos(piecePos + 17, piecePos));
	}
	if (piecePos > 16 && (board[piecePos - 17] == 13) && !(find(pinnedPiecesSet.begin(), pinnedPiecesSet.end(), piecePos - 17) != pinnedPiecesSet.end()) //-17
		&& ((piecePos - 17) / 8 == pieceRow - 2))
	{
		moves.push_back(Move::getInt16FromPos(piecePos - 17, piecePos));
	}
	if (piecePos + 15 < 64 && (board[piecePos + 15] == 13) && !(find(pinnedPiecesSet.begin(), pinnedPiecesSet.end(), piecePos + 15) != pinnedPiecesSet.end())//15
		&& ((piecePos + 15) / 8 == pieceRow + 2))
	{
		moves.push_back(Move::getInt16FromPos(piecePos + 15, piecePos));
	}
	if (piecePos > 14 && (board[piecePos - 15] == 13) && !(find(pinnedPiecesSet.begin(), pinnedPiecesSet.end(), piecePos - 15) != pinnedPiecesSet.end())//-15
		&& ((piecePos - 15) / 8 == pieceRow - 2))
	{
		moves.push_back(Move::getInt16FromPos(piecePos - 15, piecePos));
	}
	if (piecePos + 10 < 64 && (board[piecePos + 10] == 13) && !(find(pinnedPiecesSet.begin(), pinnedPiecesSet.end(), piecePos + 10) != pinnedPiecesSet.end())//10
		&& ((piecePos + 10) / 8 == pieceRow + 1))
	{
		moves.push_back(Move::getInt16FromPos(piecePos + 10, piecePos));
	}
	if (piecePos > 9 && (board[piecePos - 10] == 13) && !(find(pinnedPiecesSet.begin(), pinnedPiecesSet.end(), piecePos - 10) != pinnedPiecesSet.end())//-10
		&& ((piecePos - 10) / 8 == pieceRow - 1))
	{
		moves.push_back(Move::getInt16FromPos(piecePos - 10, piecePos));
	}
	if (piecePos + 6 < 64 && (board[piecePos + 6] == 13) && !(find(pinnedPiecesSet.begin(), pinnedPiecesSet.end(), piecePos + 6) != pinnedPiecesSet.end())//6
		&& ((piecePos + 6) / 8 == pieceRow + 1))
	{
		moves.push_back(Move::getInt16FromPos(piecePos + 6, piecePos));
	}
	if (piecePos > 5 && (board[piecePos - 6] == 13) && !(find(pinnedPiecesSet.begin(), pinnedPiecesSet.end(), piecePos - 6) != pinnedPiecesSet.end())//-6
		&& ((piecePos - 6) / 8 == pieceRow - 1))
	{
		moves.push_back(Move::getInt16FromPos(piecePos - 6, piecePos));
	}
	int pieceColumn = piecePos % 8;
	int index;
	for (size_t i = 0; i < 64; i++) //Find black sliding pieces
	{
		if (find(pinnedPiecesSet.begin(), pinnedPiecesSet.end(), i) != pinnedPiecesSet.end())
			continue;
		//Check whether they align with the white king
		switch (board[i])
		{
		case 12: //Rook [FULLY FUNCTIONAL]
			if (i % 8 == pieceColumn) //Same column
			{
				//King could be threatened by this piece. Do raycast
				//Check if king is above or below
				if (i > piecePos) //Black rook moving UP
				{
					index = i - 8;
					//Check move until piece is reached
					while (index != piecePos)
					{
						if (board[index] != 0) //If it encounters something
						{
							break;
						}
						index = index - 8;
					}
					if (index == piecePos)
					{
						moves.push_back(Move::getInt16FromPos(i, piecePos));
					}
				}
				else //Other direction, black rook moving DOWN
				{
					index = i + 8;
					//Check move until king is reached. There has to be a piece inbetween somewhere
					while (index != piecePos)
					{
						if (board[index] != 0) //If it encounters something
						{
							break;
						}
						index = index + 8;
					}
					if (index == piecePos)
					{
						//No piece inbetween! The piece is in check!
						moves.push_back(Move::getInt16FromPos(i, piecePos));
					}
				}

			}
			else if (i / 8 == pieceRow) //Same row
			{
				//King could be threatened by this piece. Do raycast
				//Check if king is above or below
				if (i > piecePos) //Black rook moving LEFT
				{
					index = i - 1;
					//Check move until king is reached. There has to be a piece inbetween somewhere
					while (index != piecePos)
					{
						if (board[index] != 0) //If it encounters something
						{
							break;
						}
						index = index - 1;
					}
					if (index == piecePos)
					{
						//No piece inbetween! The piece is in check!
						moves.push_back(Move::getInt16FromPos(i, piecePos));
					}
				}
				else //Other direction, black rook moving RIGHT
				{
					index = i + 1;
					//Check move until king is reached. There has to be a piece inbetween somewhere
					while (index != piecePos)
					{
						if (board[index] != 0) //If it encounters something
						{
							break;
						}
						index = index + 1;
					}
					if (index == piecePos)
					{
						//No piece inbetween! The piece is in check!
						moves.push_back(Move::getInt16FromPos(i, piecePos));
					}
				}
			}
			break;
		case 14: //Bishop [FULLY FUNCTIONAL]
			if (diagonalArray[i] == diagonalArray[piecePos]) //Diagonal: main file a1h8
			{
				if (i > piecePos) //Black bishop moving diagonal UP
				{
					index = i - 9;
					//Check move until king is reached. There has to be a piece inbetween somewhere
					while (index != piecePos)
					{
						if (board[index] != 0) //If it encounters something
						{
							break;
						}
						index = index - 9;
					}
					if (index == piecePos)
					{
						//No piece inbetween! The piece is in check!
						moves.push_back(Move::getInt16FromPos(i, piecePos));
					}
				}
				else //Other direction, black rook moving DOWN
				{
					index = i + 9;
					//Check move until king is reached. There has to be a piece inbetween somewhere
					while (index != piecePos)
					{
						if (board[index] != 0) //If it encounters something
						{
							break;
						}
						index = index + 9;
					}
					if (index == piecePos)
					{
						//No piece inbetween! The piece is in check!
						moves.push_back(Move::getInt16FromPos(i, piecePos));
					}
				}

			}
			else if (antiDiagonalArray[i] == antiDiagonalArray[piecePos])
			{
				if (i > piecePos) //Black bishop moving diagonal UP
				{
					index = i - 7;
					//Check move until king is reached. There has to be a piece inbetween somewhere
					while (index != piecePos)
					{
						if (board[index] != 0) //If it encounters something
						{
							break;
						}
						index = index - 7;
					}
					if (index == piecePos)
					{
						//No piece inbetween! The piece is in check!
						moves.push_back(Move::getInt16FromPos(i, piecePos));
					}
				}
				else //Other direction, black rook moving DOWN
				{
					index = i + 7;
					//Check move until king is reached. There has to be a piece inbetween somewhere
					while (index != piecePos)
					{
						if (board[index] != 0) //If it encounters something
						{
							break;
						}
						index = index + 7;
					}
					if (index == piecePos)
					{
						//No piece inbetween! The piece is in check!
						moves.push_back(Move::getInt16FromPos(i, piecePos));
					}
				}

			}
			break;
		case 15: //Queen [FULLY FUNCTIONAL]
			if (i % 8 == pieceColumn) //Same column
			{
				//King could be threatened by this piece. Do raycast
				//Check if king is above or below
				if (i > piecePos) //Black rook moving UP
				{
					index = i - 8;
					//Check move until piece is reached
					while (index != piecePos)
					{
						if (board[index] != 0) //If it encounters something
						{
							break;
						}
						index = index - 8;
					}
					if (index == piecePos)
					{
						//No piece inbetween! The piece is in check!
						moves.push_back(Move::getInt16FromPos(i, piecePos));
					}
				}
				else //Other direction, black rook moving DOWN
				{
					index = i + 8;
					//Check move until king is reached. There has to be a piece inbetween somewhere
					while (index != piecePos)
					{
						if (board[index] != 0) //If it encounters something
						{
							break;
						}
						index = index + 8;
					}
					if (index == piecePos)
					{
						//No piece inbetween! The piece is in check!
						moves.push_back(Move::getInt16FromPos(i, piecePos));
					}
				}

			}
			else if (i / 8 == pieceRow) //Same row
			{
				//King could be threatened by this piece. Do raycast
				//Check if king is above or below
				if (i > piecePos) //Black rook moving LEFT
				{
					index = i - 1;
					//Check move until king is reached. There has to be a piece inbetween somewhere
					while (index != piecePos)
					{
						if (board[index] != 0) //If it encounters something
						{
							break;
						}
						index = index - 1;
					}
					if (index == piecePos)
					{
						//No piece inbetween! The piece is in check!
						moves.push_back(Move::getInt16FromPos(i, piecePos));
					}
				}
				else //Other direction, black rook moving RIGHT
				{
					index = i + 1;
					//Check move until king is reached. There has to be a piece inbetween somewhere
					while (index != piecePos)
					{
						if (board[index] != 0) //If it encounters something
						{
							break;
						}
						index = index + 1;
					}
					if (index == piecePos)
					{
						//No piece inbetween! The piece is in check!
						moves.push_back(Move::getInt16FromPos(i, piecePos));
					}
				}
			}
			else if (diagonalArray[i] == diagonalArray[piecePos]) //Diagonal: main file a1h8
			{
				if (i > piecePos) //Black bishop moving diagonal UP
				{
					index = i - 9;
					//Check move until king is reached. There has to be a piece inbetween somewhere
					while (index != piecePos)
					{
						if (board[index] != 0) //If it encounters something
						{
							break;
						}
						index = index - 9;
					}
					if (index == piecePos)
					{
						//No piece inbetween! The piece is in check!
						moves.push_back(Move::getInt16FromPos(i, piecePos));
					}
				}
				else //Other direction, black rook moving DOWN
				{
					index = i + 9;
					//Check move until king is reached. There has to be a piece inbetween somewhere
					while (index != piecePos)
					{
						if (board[index] != 0) //If it encounters something
						{
							break;
						}
						index = index + 9;
					}
					if (index == piecePos)
					{
						//No piece inbetween! The piece is in check!
						moves.push_back(Move::getInt16FromPos(i, piecePos));
					}
				}

			}
			else if (antiDiagonalArray[i] == antiDiagonalArray[piecePos])
			{
				if (i > piecePos) //Black bishop moving diagonal UP
				{
					index = i - 7;
					//Check move until king is reached. There has to be a piece inbetween somewhere
					while (index != piecePos)
					{
						if (board[index] != 0) //If it encounters something
						{
							break;
						}
						index = index - 7;
					}
					if (index == piecePos)
					{
						//No piece inbetween! The piece is in check!
						moves.push_back(Move::getInt16FromPos(i, piecePos));
					}
				}
				else //Other direction, black rook moving DOWN
				{
					index = i + 7;
					//Check move until king is reached. There has to be a piece inbetween somewhere
					while (index != piecePos)
					{
						if (board[index] != 0) //If it encounters something
						{
							break;
						}
						index = index + 7;
					}
					if (index == piecePos)
					{
						//No piece inbetween! The piece is in check!
						moves.push_back(Move::getInt16FromPos(i, piecePos));
					}
				}

			}
			break;
		}
	}
}

ArrayBoard::ArrayBoard()
{
}

ArrayBoard::ArrayBoard(const ArrayBoard &copy) //Deep copy
{
	//Better array copying method?
	for (size_t i = 0; i < 64; i++)
	{
		board[i] = copy.board[i];
	}
	castlingBlackKingSide = copy.castlingBlackKingSide;
	castlingBlackQueenSide = copy.castlingBlackQueenSide;
	castlingWhiteKingSide = copy.castlingWhiteKingSide;
	castlingWhiteQueenSide = copy.castlingWhiteQueenSide;
	drawCounter = copy.drawCounter;
	enPassantSquare = copy.enPassantSquare;
	totalPly = copy.totalPly;
	whiteTurn = copy.whiteTurn;
}

ArrayBoard::ArrayBoard(string fenString)
{
}


ArrayBoard::~ArrayBoard()
{
}
