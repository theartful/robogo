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

    BoardSimpleGUI();

    virtual uint32_t generate_move(const Game& game);

private:
    const int BOARD_SIZE = 19;
    
    // clears console
    void clear_screen();
    
    // utility to transform data read into x and y
    void read_position(std::stringstream& stream, int& x,int &y);

    // transforms position from alphanumeric to x and y
    void get_index(char column, int row, int& x, int& y);

    // prints info of game state (Board, turn, players score)
    void print_game_state(const go::engine::GameState& game_state);

    // prints board to console
    void print_board(const go::engine::BoardState& board);

    // prints cluster liberties
    void print_liberties(const go::engine::Cluster& cluster);

    // prints cluster information
    void print_cluster_info(uint32_t index,const go::engine::ClusterTable& table);
    
    // returns list of indices of cluster
    void get_cluster_indices(std::vector<int>& indices,const go::engine::ClusterTable& table,int parent_idx);

    // transforms x and y to alphanumeric position
    inline std::string get_alphanumeric_position(int x,int y);

    // utility function to get symbol on board
    inline char get_board_symbol(go::engine::Cell cell, int x, int y);

    // checks if index is key point
    inline bool is_special(int idx);

    // transforms string into lower case
    void to_lower(std::string& str);

    // blocks reading input from user
    void get_user_input(std::string& user_input);
};

}// namespace simplegui
}// namespace go

#endif  // SRC_SIMPLEGUI_SIMPLEGUI_H_
