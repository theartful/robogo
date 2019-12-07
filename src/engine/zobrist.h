#ifndef SRC_ZOBRIST_H_
#define SRC_ZOBRIST_H_

#include <random>
#include <unordered_map>

#include "board.h"

typedef unsigned long long int int64;

extern std::mt19937_64 mt_64;

namespace go
{

namespace engine
{

struct Zobrist
{

	int64 hash[8][4][BoardState::MAX_NUM_CELLS];

	Zobrist()
	{

		std::mt19937_64 mt_64_(123);

		for (int i = 0; i < 8; ++i)
		{

			for (int j = 0; j < 4; ++j)
			{

				for (int x = 0; x < BoardState::EXTENDED_BOARD_SIZE; ++x)
				{

					for (int y = 0; y < BoardState::EXTENDED_BOARD_SIZE; ++y)
					{

						if (i == 0)
							hash[i][j]
							    [y + BoardState::EXTENDED_BOARD_SIZE * x] =
							        mt_64_();
						else if (i == 4)
							hash[i][j]
							    [y + BoardState::EXTENDED_BOARD_SIZE * x] =
							        hash[0][j]
							            [y +
							             BoardState::EXTENDED_BOARD_SIZE * x];
						else
							hash[i][j]
							    [y + BoardState::EXTENDED_BOARD_SIZE * x] =
							        hash[i - 1][j]
							            [x +
							             BoardState::EXTENDED_BOARD_SIZE * y];
					}
				}
			}
		}
	}
};
const Zobrist zobrist;

class BoardState;

int64 BoardHash(BoardState& b);

} // namespace engine

} // namespace go

#endif // SRC_ZOBRIST_H_