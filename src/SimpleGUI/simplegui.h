#ifndef SRC_SIMPLEGUI_SIMPLEGUI_H_
#define SRC_SIMPLEGUI_SIMPLEGUI_H_
#include "../engine/board.h"
#include "../engine/cluster.h"

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

    void print_cluster_info(const go::engine::Cluster& cluster);

    void get_user_input(std::string& user_input);
};

}// namespace simplegui
}// namespace go

#endif  // SRC_SIMPLEGUI_SIMPLEGUI_H_
