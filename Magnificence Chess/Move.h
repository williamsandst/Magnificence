#pragma once

//Static class for generating int16 move objects
class Move
{
public:
	static __int16 getInt16FromPos(int start, int end);
	static __int16 getInt16FromPos(int start, int end, int promotion, int type);
	static int getStartPosFromInt16(__int16 move);
	static int getEndPosFromInt16(__int16 move);

	static int getFrom(__int16 *move);
	static int getTo(__int16 *move);
	static int getPromotion(__int16 &move);
	static int getType(__int16 &move);
private:
	Move();
	~Move();
};

