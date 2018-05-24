
//Static class for evaluation
#include "BitBoard.h"

class Evaluation
{
public:
	static int lazyEval(BitBoard * bb);
	static void getPhase(BitBoard * bb);
	static int pieceSquareValues(const short * pieceSquareTable, u64 pieceSet);
	static short getPieceValue(u8 piece);

	static short getPieceValueColor(u8 piece);

private:
	const static int Bishop = 300, Rook = 500, Knight = 300, Queen = 900, Pawn = 100;

	const static short whiteBishopEarlyPST[64];

	const static short blackBishopEarlyPST[64];

	const static short whiteBishopLatePST[64];

	const static short blackBishopLatePST[64];

	const static short whitePawnEarlyPST[64];

	const static short blackPawnEarlyPST[64];

	const static short whitePawnLatePST[64];

	const static short blackPawnLatePST[64];

	const static short whiteKnightEarlyPST[64];

	const static short blackKnightEarlyPST[64];

	const static short whiteKnightLatePST[64];

	const static short blackKnightLatePST[64];

	const static short whiteRookEarlyPST[64];

	const static short blackRookEarlyPST[64];

	const static short whiteRookLatePST[64];

	const static short blackRookLatePST[64];

	const static short whiteQueenEarlyPST[64];

	const static short blackQueenEarlyPST[64];

	const static short whiteQueenLatePST[64];

	const static short blackQueenLatePST[64];

	const static short whiteKingEarlyPST[64];

	const static short blackKingEarlyPST[64];

	const static short whiteKingLatePST[64];

	const static short blackKingLatePST[64];

	Evaluation();
	~Evaluation();
};

