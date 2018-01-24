#pragma once
#include <vector>
#include <string>

using namespace std;

//Base class for Board
//Board represents the chess board and handles moves
class Board
{
public:
	bool whiteTurn;
	Board();
	Board(string fenString); //Fen string to give a starting position
	Board(const Board &Board); //Copy
	~Board();
	void makeMove(__int16 move); //
	vector<__int16> generateWhiteLegalMoves();
	vector<__int16> generateBlackLegalMoves();
};