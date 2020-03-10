#pragma once

#include <inttypes.h>

typedef uint_fast64_t  u64;
typedef uint_least32_t u32;
typedef uint_fast16_t u16;
typedef int_fast16_t i16;
typedef uint_fast8_t u8;

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

