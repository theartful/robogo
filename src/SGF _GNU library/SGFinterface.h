#include "engine/board.h"
// #include "myBoard.h"
#include "sgftree.h"
#include <cstring>

void get_sgf_move(SGFProperty* prop, int& x, int& y, bool& isPass);
void sgf_play_node(SGFNode *node, uint32_t& index, uint32_t& player, bool& has_move, bool& isPass);
std::vector<go::engine::Action> load_sgf_tree(SGFNode* head);
void get_move_x_y(uint32_t index, char* x, char* y);
void extract_sgf_file(std::vector<go::engine::Action>& moves, const char* file_name);

