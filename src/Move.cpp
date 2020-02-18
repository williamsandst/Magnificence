#include "stdafx.h"
#include "Move.h"


//Static class supporting Move objects (int16)

u32 Move::getInt32FromPos(u32 start, u32 end) //Encoding move information in int16 to save space
{
	return (end | (start << 6));
}

int Move::getFrom(u32 move)
{
	return (move >> 6) & 63;
}

int Move::getTo(u32 move)
{
	return move & 63;
}

//Static class, declaration not used
Move::Move()
{
}


Move::~Move()
{
}
