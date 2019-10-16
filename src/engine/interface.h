#ifndef _INTERFACE_H_
#define _INTERFACE_H_

#include "board.h"

namespace go
{
    namespace engine
    {
        // Plays a move, if legal, changing the board state and game state
        bool make_move(GameState&, const Action&);
        // Plays a move, if legal, changing the board state
        bool make_move(BoardState&, const Action&);
        bool is_valid_move(BoardState&, const Action&);
        uint32_t calculate_score(BoardState&, Player);
    }
}

#endif // _INTERFACE_H_
