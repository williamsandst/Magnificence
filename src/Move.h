#pragma once
#include "stdafx.h"

typedef unsigned long long int u64;
typedef unsigned long int u32;
typedef unsigned short int u16;
typedef unsigned char u8;

//Static class for generating int16 move objects
class Move
{
public:
	static u32 getInt32FromPos(u32 start, u32 end);
	static int getFrom(u32 move);
	static int getTo(u32 move);

private:
	Move();
	~Move(); 
};

