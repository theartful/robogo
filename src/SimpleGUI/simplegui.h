#ifndef SRC_SIMPLEGUI_SIMPLEGUI_H_
#define SRC_SIMPLEGUI_SIMPLEGUI_H_
#include "engine/board.h"
#include "engine/cluster.h"
#include "controller/agent.h"
#include "controller/game.h"
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>

namespace go{

namespace simplegui{

class BoardSimpleGUI: public Agent
{
public:

    virtual uint32_t generate_move(const Game& game);

private:
    const int BOARD_SIZE = 19;
    
    BoardSimpleGUI();
    
    // clears console
    void clear_screen();

    void get_user_input(std::string& user_input);
    
    void read_position(std::stringstream& stream, int& x,int &y);

    void get_index(char column, int row, int& x, int& y);

    // prints info of game state (Board, turn, players score)
    void print_game_state(const go::engine::GameState& game_state);

    //print board to console
    void print_board(const go::engine::BoardState& board);

    void print_liberties(const go::engine::Cluster& cluster);

    void print_cluster_info(int index,const go::engine::ClusterTable& table);
    
    void get_cluster_indices(std::vector<int>& indices,const go::engine::ClusterTable& table,int parent_idx);

    inline std::string get_alphanumeric_position(int x,int y);

    inline char get_board_symbol(go::engine::Cell cell, int x, int y);

    inline bool is_special(int idx);

    void to_lower(std::string& str);

};

}// namespace simplegui
}// namespace go

#endif  // SRC_SIMPLEGUI_SIMPLEGUI_H_
