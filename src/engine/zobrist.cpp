#include "zobrist.h"
#include "board.h"

std::random_device rd;
std::mt19937_64 mt_64(rd());
std::uniform_int_distribution<int> mt_int8(0, 7);

int64 go::engine::BoardHash(BoardState& b)
{

	int64 b_hash = 0;

	for (int i; i < BoardState::MAX_NUM_CELLS; i++)
	{
		switch (b.board[i])
		{
		case Cell::WHITE:
			b_hash ^= zobrist.hash[0][1][i];
			break;
		case Cell::BLACK:
			b_hash ^= zobrist.hash[0][2][i];
			break;
		default:
			break;
		}
	}

	// XOR if Ko
	if (b.ko != 0)
		b_hash ^= zobrist.hash[0][3][b.ko];

	// TODO: XOR with player's turn

	return b_hash;
}
