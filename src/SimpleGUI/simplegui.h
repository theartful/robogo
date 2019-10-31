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

    // helper function to get symbol for players
    inline char get_board_symbol(go::engine::Cell cell);

    // prints info of game state (Board, turn, players score)
    void print_game_state(const go::engine::GameState& game_state);

    //print board to console, requires clearing console
    void print_board(const go::engine::BoardState& board);

    void print_cluster_info(const go::engine::Cluster& cluster);

    void get_user_input(std::string& user_input);
};

}// namespace simplegui
}// namespace go

#endif  // SRC_SIMPLEGUI_SIMPLEGUI_H_
