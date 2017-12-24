#include "stdafx.h"
#include "Move.h"
//Static class supporting Move objects (int16)

__int16 Move::getInt16FromPos(int start, int end) //Encoding move information in int16 to save space
{
	return (start << 9) | (end << 2);
}

int Move::getStartPosFromInt16(__int16 move)
{
	return (move >> 9) & 127;
}

int Move::getEndPosFromInt16(__int16 move)
{
	return (move >> 2) & 127;
}

//Static class, declaration not used
Move::Move()
{
}


Move::~Move()
{
}
