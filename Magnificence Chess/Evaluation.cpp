#include "stdafx.h"
#include "Evaluation.h"
#include "Settings.h"


//Returns an aproximate score based on material and pieceSquareTables
int Evaluation::lazyEval(BitBoard *bb)
{
	//nodes[0]++;
	getPhase(bb);
	//phase = 255;
	short materialScore = 0;
	short earlyPSTScore = 0;
	short latePSTScore = 0;
	materialScore += bb->pc(bb->Pieces[5]) * Pawn + bb->pc(bb->Pieces[4]) * Knight + bb->pc(bb->Pieces[3]) * Rook + bb->pc(bb->Pieces[2]) * Bishop + bb->pc(bb->Pieces[1]) * Queen
		- bb->pc(bb->Pieces[12]) * Pawn - bb->pc(bb->Pieces[11]) * Knight - bb->pc(bb->Pieces[10]) * Rook - bb->pc(bb->Pieces[9]) * Bishop - bb->pc(bb->Pieces[8]) * Queen;
	
	earlyPSTScore += pieceSquareValues(whitePawnEarlyPST, bb->Pieces[5]);
	earlyPSTScore -= pieceSquareValues(blackPawnEarlyPST, bb->Pieces[12]);

	earlyPSTScore += pieceSquareValues(whiteKnightEarlyPST, bb->Pieces[4]);
	earlyPSTScore -= pieceSquareValues(blackKnightEarlyPST, bb->Pieces[11]);

	earlyPSTScore += pieceSquareValues(whiteBishopEarlyPST, bb->Pieces[2]);
	earlyPSTScore -= pieceSquareValues(blackBishopEarlyPST, bb->Pieces[9]);

	earlyPSTScore += pieceSquareValues(whiteRookEarlyPST, bb->Pieces[3]);
	earlyPSTScore -= pieceSquareValues(blackRookEarlyPST, bb->Pieces[10]);

	earlyPSTScore += pieceSquareValues(whiteQueenEarlyPST, bb->Pieces[1]);
	earlyPSTScore -= pieceSquareValues(blackQueenEarlyPST, bb->Pieces[8]);

	earlyPSTScore += pieceSquareValues(whiteKingEarlyPST, bb->Pieces[0]);
	earlyPSTScore -= pieceSquareValues(blackKingEarlyPST, bb->Pieces[7]);

	//End scores
	latePSTScore += pieceSquareValues(whitePawnLatePST, bb->Pieces[5]);
	latePSTScore -= pieceSquareValues(blackPawnLatePST, bb->Pieces[12]);

	latePSTScore += pieceSquareValues(whiteKnightLatePST, bb->Pieces[4]);
	latePSTScore -= pieceSquareValues(blackKnightLatePST, bb->Pieces[11]);

	latePSTScore += pieceSquareValues(whiteBishopLatePST, bb->Pieces[2]);
	latePSTScore -= pieceSquareValues(blackBishopLatePST, bb->Pieces[9]);

	latePSTScore += pieceSquareValues(whiteRookLatePST, bb->Pieces[3]);
	latePSTScore -= pieceSquareValues(blackRookLatePST, bb->Pieces[10]);

	latePSTScore += pieceSquareValues(whiteQueenLatePST, bb->Pieces[1]);
	latePSTScore -= pieceSquareValues(blackQueenLatePST, bb->Pieces[8]);

	latePSTScore += pieceSquareValues(whiteKingLatePST, bb->Pieces[0]);
	latePSTScore -= pieceSquareValues(blackKingLatePST, bb->Pieces[7]);

	//0 = earlyPSTScore
	//256 = latePSTScore
	//0 - 256
	int phaseScore = ((256 - bb->phase)*earlyPSTScore + bb->phase * latePSTScore) / 256;
	if (bb->color) materialScore += 20;
	return materialScore + phaseScore;
}

void Evaluation::getPhase(BitBoard *bb)
{
	const int pawnPhase = 0;
	const int knightPhase = 1;
	const int bishopPhase = 1;
	const int rookPhase = 2;
	const int queenPhase = 4;
	int totalPhase = pawnPhase * 16 + knightPhase * 4 + bishopPhase * 4 + rookPhase * 4 + queenPhase * 2;

	int phase = totalPhase;

	//White pieces
	phase -= bb->pc(bb->Pieces[5]) * pawnPhase;
	phase -= bb->pc(bb->Pieces[4]) * knightPhase;
	phase -= bb->pc(bb->Pieces[2]) * bishopPhase;
	phase -= bb->pc(bb->Pieces[3]) * rookPhase;
	phase -= bb->pc(bb->Pieces[1]) * queenPhase;

	//Black pieces
	phase -= bb->pc(bb->Pieces[12]) * pawnPhase;
	phase -= bb->pc(bb->Pieces[11]) * knightPhase;
	phase -= bb->pc(bb->Pieces[9]) * bishopPhase;
	phase -= bb->pc(bb->Pieces[10]) * rookPhase;
	phase -= bb->pc(bb->Pieces[8]) * queenPhase;

	phase = (phase * 256 + (totalPhase / 2)) / totalPhase;
	bb->phase = phase;
}

//Used in eval to extract values from the piece square tables for positional awareness 
int Evaluation::pieceSquareValues(const short * pieceSquareTable, u64 pieceSet)
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

short Evaluation::getPieceValue(u8 piece)
{
	switch (piece)
	{
	case 14:
		return 0;
		break;
	case 7:
	case 0:
		return 32000;
		break;
	case 1:
	case 8:
		return Queen;
		break;
	case 2:
	case 9:
		return Bishop;
		break;
	case 3:
	case 10:
		return Rook;
		break;
	case 4:
	case 11:
		return Knight;
		break;
	case 5:
	case 12:
		return Pawn;
		break;
	default:
		return 0;
		break;
	}
	return 0;
}

short Evaluation::getPieceValueColor(u8 piece)
{
	switch (piece)
	{
	case 14:
		return 0;
		break;
	case 1:
		return Queen;
		break;
	case 8:
		return -Queen;
		break;
	case 2:
		return Bishop;
		break;
	case 9:
		return -Bishop;
		break;
	case 3:
		return Rook;
		break;
	case 10:
		return -Rook;
		break;
	case 4:
		return Knight;
		break;
	case 11:
		return -Knight;
		break;
	case 5:
		return Pawn;
		break;
	case 12:
		return -Pawn;
		break;
	default:
		return 0;
		break;
	}
	return 0;
}

//Will never run, static class
Evaluation::Evaluation()
{
}


Evaluation::~Evaluation()
{
}

//Piece square tables
const short Evaluation::whiteBishopEarlyPST[64] =
	{ 19,  16,  17,  18,  18,  17,  16,  19,
	 -14,  23,  20,  21,  21,  20,  23, -14,
	  17,  20,  26,  23,  23,  26,  20,  17,
	  18,  21,  23,  28,  28,  23,  21,  18,
	  18,  21,  23,  28,  28,  23,  21,  18,
	  17,  20,  26,  23,  23,  26,  20,  17,
	  16,  23,  20,  21,  21,  20,  23,  16,
	  9,   6,   7,   8,   8,   7,   6,   9, };

	const short Evaluation::blackBishopEarlyPST[64] =
	{ 9,  6,  7,  8,  8,  7,  6,  9,
		16, 23, 20, 21, 21, 20, 23, 16,
		17, 20, 26, 23, 23, 26, 20, 17,
		18, 21, 23, 28, 28, 23, 21, 18,
		18, 21, 23, 28, 28, 23, 21, 18,
		17, 20, 26, 23, 23, 26, 20, 17,
		-14, 23, 20, 21, 21, 20, 23,-14,
		19, 16, 17, 18, 18, 17, 16, 19, };

	const short Evaluation::whiteBishopLatePST[64] =
	{ 20, 22, 24, 26, 26, 24, 22, 20,
		22, 28, 30, 32, 32, 30, 28, 22,
		24, 30, 34, 36, 36, 34, 30, 24,
		26, 32, 36, 38, 38, 36, 32, 26,
		26, 32, 36, 38, 38, 36, 32, 26,
		24, 30, 34, 36, 36, 34, 30, 24,
		22, 28, 30, 32, 32, 30, 28, 22,
		20, 22, 24, 26, 26, 24, 22, 20 };

	const short Evaluation::blackBishopLatePST[64] =
	{ 20, 22, 24, 26, 26, 24, 22, 20,
		22, 28, 30, 32, 32, 30, 28, 22,
		24, 30, 34, 36, 36, 34, 30, 24,
		26, 32, 36, 38, 38, 36, 32, 26,
		26, 32, 36, 38, 38, 36, 32, 26,
		24, 30, 34, 36, 36, 34, 30, 24,
		22, 28, 30, 32, 32, 30, 28, 22,
		20, 22, 24, 26, 26, 24, 22, 20 };

	//Done!
	const short Evaluation::whitePawnEarlyPST[64] =
	{ 0,   5,  10,  15,  15,  10,   5,   0,
		2,   7,  12,  -5,  -5,  12,   7,   2,
		0,   5,  10,  15,  15,  10,   5,   0,
		0,   5,  10,  35,  35,  10,   5,   0,
		0,   5,  10,  25,  25,  10,   5,   0,
		0,   5,  10,  15,  15,  10,   5,   0,
		0,   5,  10,  15,  15,  10,   5,   0,
		0,   5,  10,  15,  15,  10,   5,   0, };

	//Done!
	const short Evaluation::blackPawnEarlyPST[64] =
	{ 0,   5,  10,  15,  15,  10,   5,   0,
		0,   5,  10,  15,  15,  10,   5,   0,
		0,   5,  10,  15,  15,  10,   5,   0,
		0,   5,  10,  25,  25,  10,   5,   0,
		0,   5,  10,  35,  35,  10,   5,   0,
		0,   5,  10,  15,  15,  10,   5,   0,
		2,   7,  12,  -5,  -5,  12,   7,   2,
		0,   5,  10,  15,  15,  10,   5,   0, };

	//Done!
	const short Evaluation::whitePawnLatePST[64] =
	{ 0,  0,  0,  0,  0,  0,  0,  0,
		0,  0,  0,  0,  0,  0,  0,  0,
		0,  0,  0,  0,  0,  0,  0,  0,
		0,  0,  0,  0,  0,  0,  0,  0,
		0,  0,  0,  0,  0,  0,  0,  0,
		0,  0,  0,  0,  0,  0,  0,  0,
		0,  0,  0,  0,  0,  0,  0,  0,
		0,  0,  0,  0,  0,  0,  0,  0 };

	//Done!
	const short Evaluation::blackPawnLatePST[64] =
	{ 0,  0,  0,  0,  0,  0,  0,  0,
		0,  0,  0,  0,  0,  0,  0,  0,
		0,  0,  0,  0,  0,  0,  0,  0,
		0,  0,  0,  0,  0,  0,  0,  0,
		0,  0,  0,  0,  0,  0,  0,  0,
		0,  0,  0,  0,  0,  0,  0,  0,
		0,  0,  0,  0,  0,  0,  0,  0,
		0,  0,  0,  0,  0,  0,  0,  0 };

	//Done!
	const short Evaluation::whiteKnightEarlyPST[64] =
	{ -15, -12,  -9,  -6,  -6,  -9, -12, -15,
		3,  12,  15,  18,  18,  15,  12,   3,
		6,  15,  21,  24,  24,  21,  15,   6,
		9,  18,  24,  27,  27,  24,  18,   9,
		9,  18,  27,  32,  32,  27,  18,   9,
		6,  15,  21,  27,  27,  21,  15,   6,
		3,  12,  15,  18,  18,  15,  12,   3,
		-50,   3,   6,   9,   9,   6,   3, -50 };

	//Done!
	const short Evaluation::blackKnightEarlyPST[64] =
	{ -50,   3,   6,   9,   9,   6,   3, -50,
		3,  12,  15,  18,  18,  15,  12,   3,
		6,  15,  21,  27,  27,  21,  15,   6,
		9,  18,  27,  32,  32,  27,  18,   9,
		9,  18,  24,  27,  27,  24,  18,   9,
		6,  15,  21,  24,  24,  21,  15,   6,
		3,  12,  15,  18,  18,  15,  12,   3,
		-15, -12,  -9,  -6,  -6,  -9, -12, -15 };

	//Done!
	const short Evaluation::whiteKnightLatePST[64] =
	{ 0,  3,  6,  9,  9,  6,  3,  0,
		3, 12, 15, 18, 18, 15, 12,  3,
		6, 15, 21, 24, 24, 21, 15,  6,
		9, 18, 24, 27, 27, 24, 18,  9,
		9, 18, 24, 27, 27, 24, 18,  9,
		6, 15, 21, 24, 24, 21, 15,  6,
		3, 12, 15, 18, 18, 15, 12,  3,
		0,  3,  6,  9,  9,  6,  3,  0, };

	//Done!
	const short Evaluation::blackKnightLatePST[64] =
	{ 0,  3,  6,  9,  9,  6,  3,  0,
		3, 12, 15, 18, 18, 15, 12,  3,
		6, 15, 21, 24, 24, 21, 15,  6,
		9, 18, 24, 27, 27, 24, 18,  9,
		9, 18, 24, 27, 27, 24, 18,  9,
		6, 15, 21, 24, 24, 21, 15,  6,
		3, 12, 15, 18, 18, 15, 12,  3,
		0,  3,  6,  9,  9,  6,  3,  0, };

	const short Evaluation::whiteRookEarlyPST[64] =
	{ 0,   3,   6,   9,   9,   6,   3,   0,
	  25,  28,  31,  34,  34,  31,  28,  25,
	  0,   3,   6,   9,   9,   6,   3,   0,
	  0,   3,   6,   9,   9,   6,   3,   0,
	  0,   3,   6,   9,   9,   6,   3,   0,
	  0,   3,   6,   9,   9,   6,   3,   0,
	  0,   3,   6,   9,   9,   6,   3,   0,
	  1,   4,   7,  10,  10,   7,   4,   1, };

	const short Evaluation::blackRookEarlyPST[64] =
	{ 1,  4,  7, 10, 10,  7,  4,  1,
		0,  3,  6,  9,  9,  6,  3,  0,
		0,  3,  6,  9,  9,  6,  3,  0,
		0,  3,  6,  9,  9,  6,  3,  0,
		0,  3,  6,  9,  9,  6,  3,  0,
		0,  3,  6,  9,  9,  6,  3,  0,
		25, 28, 31, 34, 34, 31, 28, 25,
		0,  3,  6,  9,  9,  6,  3,  0 };

	const short Evaluation::whiteRookLatePST[64] =
	{ 25, 25, 25, 25, 25, 25, 25, 25,
		25, 25, 25, 25, 25, 25, 25, 25,
		25, 25, 25, 25, 25, 25, 25, 25,
		25, 25, 25, 25, 25, 25, 25, 25,
		25, 25, 25, 25, 25, 25, 25, 25,
		25, 25, 25, 25, 25, 25, 25, 25,
		25, 25, 25, 25, 25, 25, 25, 25,
		25, 25, 25, 25, 25, 25, 25, 25, };

	const short Evaluation::blackRookLatePST[64] =
	{ 25, 25, 25, 25, 25, 25, 25, 25,
		25, 25, 25, 25, 25, 25, 25, 25,
		25, 25, 25, 25, 25, 25, 25, 25,
		25, 25, 25, 25, 25, 25, 25, 25,
		25, 25, 25, 25, 25, 25, 25, 25,
		25, 25, 25, 25, 25, 25, 25, 25,
		25, 25, 25, 25, 25, 25, 25, 25,
		25, 25, 25, 25, 25, 25, 25, 25, };

	const short Evaluation::whiteQueenEarlyPST[64] =
	{ 95,  95,  95,  95,  95,  95,  95,  95,
		100, 100, 100, 100, 100, 100, 100, 100,
		100, 100, 100, 100, 100, 100, 100, 100,
		100, 100, 100, 100, 100, 100, 100, 100,
		100, 100, 100, 100, 100, 100, 100, 100,
		100, 100, 100, 100, 100, 100, 100, 100,
		115, 115, 115, 115, 115, 115, 115, 115,
		100, 100, 100, 100, 100, 100, 100, 100, };

	const short Evaluation::blackQueenEarlyPST[64] =
	{ 100, 100, 100, 100, 100, 100, 100, 100,
		115, 115, 115, 115, 115, 115, 115, 115,
		100, 100, 100, 100, 100, 100, 100, 100,
		100, 100, 100, 100, 100, 100, 100, 100,
		100, 100, 100, 100, 100, 100, 100, 100,
		100, 100, 100, 100, 100, 100, 100, 100,
		100, 100, 100, 100, 100, 100, 100, 100,
		95,  95,  95,  95,  95,  95,  95,  95, };

	const short Evaluation::whiteQueenLatePST[64] =
	{ 80, 83, 86, 89, 89, 86, 83, 80,
		83, 92, 95, 98, 98, 95, 92, 83,
		86, 95,101,104,104,101, 95, 86,
		89, 98,104,107,107,104, 98, 89,
		89, 98,104,107,107,104, 98, 89,
		86, 95,101,104,104,101, 95, 86,
		83, 92, 95, 98, 98, 95, 92, 83,
		80, 83, 86, 89, 89, 86, 83, 80 };

	const short Evaluation::blackQueenLatePST[64] =
	{ 80, 83, 86, 89, 89, 86, 83, 80,
		83, 92, 95, 98, 98, 95, 92, 83,
		86, 95,101,104,104,101, 95, 86,
		89, 98,104,107,107,104, 98, 89,
		89, 98,104,107,107,104, 98, 89,
		86, 95,101,104,104,101, 95, 86,
		83, 92, 95, 98, 98, 95, 92, 83,
		80, 83, 86, 89, 89, 86, 83, 80 };

	const short Evaluation::whiteKingEarlyPST[64] =
	{   50,  50,   0,  20,   0,   0,  50,  50,
		50,  50,   0,   0,   0,   0,  50,  50,
		-50, -50, -50, -50, -50, -50, -50, -50,
		-75, -75, -75, -75, -75, -75, -75, -75,
		-100,-100,-100,-100,-100,-100,-100,-100,
		-125,-125,-125,-125,-125,-125,-125,-125,
		-150,-150,-150,-150,-150,-150,-150,-150,
		-175,-175,-175,-175,-175,-175,-175,-175, };

	const short Evaluation::blackKingEarlyPST[64] =
	{ -175,-175,-175,-175,-175,-175,-175,-175,
		-150,-150,-150,-150,-150,-150,-150,-150,
		-125,-125,-125,-125,-125,-125,-125,-125,
		-100,-100,-100,-100,-100,-100,-100,-100,
		-75, -75, -75, -75, -75, -75, -75, -75,
		-50, -50, -50, -50, -50, -50, -50, -50,
		50,  50,   0,   0,   0,   0,  50,  50,
		50,  50,   0,   20,  0,   0,  50,  50, };

	const short Evaluation::whiteKingLatePST[64] =
	{ 0, 10, 20, 30, 30, 20, 10,  0,
		10, 40, 50, 60, 60, 50, 40, 10,
		20, 50, 70, 80, 80, 70, 50, 20,
		30, 60, 80, 90, 90, 80, 60, 30,
		30, 60, 80, 90, 90, 80, 60, 30,
		20, 50, 70, 80, 80, 70, 50, 20,
		10, 40, 50, 60, 60, 50, 40, 10,
		0, 10, 20, 30, 30, 20, 10,  0 };

	const short Evaluation::blackKingLatePST[64] =
	{ 0, 10, 20, 30, 30, 20, 10,  0,
		10, 40, 50, 60, 60, 50, 40, 10,
		20, 50, 70, 80, 80, 70, 50, 20,
		30, 60, 80, 90, 90, 80, 60, 30,
		30, 60, 80, 90, 90, 80, 60, 30,
		20, 50, 70, 80, 80, 70, 50, 20,
		10, 40, 50, 60, 60, 50, 40, 10,
		0, 10, 20, 30, 30, 20, 10,  0 };