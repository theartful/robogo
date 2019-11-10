// #include "../engine/board.h"
#include "myBoard.h"
#include "sgftree.h"
#include <cstring>

void get_sgf_move(SGFProperty* prop, int& x, int& y, bool& isPass);
void sgf_play_node(SGFNode *node, int& index, int player, bool& has_move);
std::vector<Action> load_sgf_tree(SGFNode* head);

