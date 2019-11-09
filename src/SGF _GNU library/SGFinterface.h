// #include "../engine/board.h"
#include "sgftree.h"
#include <cstring>



void get_sgf_move(SGFProperty* prop, int& x, int& y);
void sgf_play_node(SGFNode *node, int& index, int player, bool& has_move);
void load_sgf_tree(SGFNode* head);

