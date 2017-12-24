#pragma once

//Base class for Board
//Board represents the chess board and handles moves
class Board
{
public:
	Board();
	~Board();
	void move(); //
	void undoMove();

};