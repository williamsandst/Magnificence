#include "stdafx.h"
#include "Move.h"
//Static class supporting Move objects (int16)

__int16 Move::getInt16FromPos(int start, int end) //Encoding move information in int16 to save space
{
	return (start << 10) | (end << 4);
}
//This function also encodes promotions and move information
//Promotions: No promotion = 0, Q = 1, R = 2, B = 3, N = 4
//Type: 0 = Quiet move, 1 = Capture, 2 = Castling, 3 = En passant
__int16 Move::getInt16FromPos(int start, int end, int promotion, int type)
{
	return (type | (promotion << 2) | (start << 10) | (end << 4));
}

int Move::getPromotion(__int16 &move)
{
	return (move >> 2) & 3;
}

int Move::getType(__int16 &move)
{
	return move & 3;
}

int Move::getStartPosFromInt16(__int16 move)
{
	return (move >> 9) & 127;
}

int Move::getEndPosFromInt16(__int16 move)
{
	return (move >> 2) & 127;
}

int Move::getFrom(__int16 * move)
{
	return (*move >> 10) & 63;
}

int Move::getTo(__int16 * move)
{
	return (*move >> 4) & 63;
}

//Static class, declaration not used
Move::Move()
{
}


Move::~Move()
{
}
