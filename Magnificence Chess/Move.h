#pragma once

//Static class for generating int16 move objects
class Move
{
public:
	__int16 getInt16FromPos(int start, int end);
	int getStartPosFromInt16(__int16 move);
	int getEndPosFromInt16(__int16 move);
private:
	Move();
	~Move();
};

