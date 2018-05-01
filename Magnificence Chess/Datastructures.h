#pragma once

typedef unsigned long long int u64;
typedef signed long long int i64;
typedef unsigned long int u32;
typedef signed long int i32;
typedef unsigned short int u16;
typedef signed short int i16;
typedef unsigned char u8;
typedef signed char i8;

struct Mover
{
private:
	u32 move;
public:
	//move definition
	//1 from taken, 1 from upgrade to, 1 rockad, 1 non silent, 1 upgrade
	//Taken 3 = T ( << 29), upgradeTo [3] = U << 26, Bit rockad [4] = R << 22, EP state [4] = s << 18, Silent [6] = S << 12, To [6] t << 6, From[6] == F << 0;
	// TTTUUURRRRssssSSSSSSttttttffffff
	// is silent if ((move >> 29) == 7)
	// is promotion if (((move >> 26) & 0b111) > 0)

	//useful masks,       TTTUUURRRRssssSSSSSSttttttffffff
	//metadata mask =	0b00000011111111111111000000000000
	//~metadatoa mask	0b11111100000000000000111111111111 

	Mover()
	{
		move = 0;
	}
	Mover(u32 in)
	{
		move = in;
	}
	Mover operator =(Mover a) 
	{ 
		move = a.move; return Mover(move);
	}
	Mover operator =(u32 a) 
	{ 
		move = a; return Mover(a); 
	}
	

	//sets square of origin
	//only defined for inputs less than or equal to 63
	//inputs greater than or equal to 64 will result in undefined behaviour
	inline void setFrom(u8 from) 
	{ 
		move = (move & (~63)) | from; 
	}

	//Returns the square of origin
	inline u8 getFrom() 
	{ 
		return (move & 63); 
	}

	//Sets square of destination
	//only defined for inputs less than or equal to 63
	//inputs greater than or equal to 64 will result in undefined behaviour
	inline void setDestination(u8 to) 
	{
		move = (move & (~(63 << 6))) | (to << 6);
	}

	//returns the square of destination
	inline u8 getDestination()
	{
		return ((move >> 6) & 63);
	}

	//returns the combined to from move structure. Useful for comparing moves. 
	inline u16 getToFrom()
	{
		return (move & 8191);
	}

	//returns true for silent moves
	inline bool checkIfSilent()
	{
		return ((move >> 29) == 7);
	}

	//returns the number of silent white moves in a row with no non silent moves before
	//that has taken place before this move. Part of metadata
	inline u8 getSilent()
	{
		return ((move >> 12) & 63);
	}

	//returns the EP line
	inline u8 getEPState()
	{
		return ((move >> 18) & 0b1111);
	}
};