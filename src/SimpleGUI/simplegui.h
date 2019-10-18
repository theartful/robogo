#ifndef SRC_SIMPLEGUI_SIMPLEGUI_H_
#define SRC_SIMPLEGUI_SIMPLEGUI_H_

namespace go{

namespace simplegui{

struct BoardSimpleGUI
{
    static const int BOARD_SIZE = 19;

    BoardSimpleGUI();

    // clears console
    void clear_screen();

    // prints info of game state (Board, turn, players score)
    void print_game_state(const GameState& game_state);

    //print board to console, requires clearing console
    void print_board(const BoardState& board);


};

}// namespace simplegui
}// namespace go

#endif  // SRC_SIMPLEGUI_SIMPLEGUI_H_
