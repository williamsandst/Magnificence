#pragma once
#include <vector>
#include <string>

using namespace std;

//Base class for Board
//Board represents the chess board and handles moves
class Board
{
public:
	Board();
	Board(string fenString); //Fen string to give a starting position
	~Board();
};