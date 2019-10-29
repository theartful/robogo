#ifndef SRC_SIMPLEGUI_SIMPLEGUI_H_
#define SRC_SIMPLEGUI_SIMPLEGUI_H_
#include "../engine/board.h"

namespace go{

namespace simplegui{

struct BoardSimpleGUI
{
    static const int BOARD_SIZE = 19;

    BoardSimpleGUI();

    // clears console
    void clear_screen();

    // helper function to get symbol for players
    inline char get_board_symbol(go::engine::Cell cell);

    // prints info of game state (Board, turn, players score)
    void print_game_state(const go::engine::GameState& game_state);

    //print board to console, requires clearing console
    void print_board(const go::engine::BoardState& board);


};

}// namespace simplegui
}// namespace go

#endif  // SRC_SIMPLEGUI_SIMPLEGUI_H_
