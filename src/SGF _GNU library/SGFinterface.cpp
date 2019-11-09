#include "SGFinterface.h"
// #include "../engine/board.h"


void get_sgf_move(SGFProperty* prop, int& x, int& y)
{
    int length = strlen(prop->value);
    if(length < 2)
    {
        perror("length of prop name less than 2\n");
    }

    x = prop->value[0] - 97;
    y = prop->value[1] - 97;

    printf("(x,y):(%d,%d)\n",x ,y);
    return;
}

void sgf_play_node(SGFNode *node, int& index, int player, bool& has_move)
{
    SGFProperty *prop;
    int x, y;
    for (prop = node->props; prop; prop = prop->next)
    {
        switch (prop->name)
        {
            case SGFB:
            case SGFW: 
            case SGFAB:
            case SGFAW:
            x,y = 0;
            get_sgf_move(prop, x, y);
            index = x + y;
            player = (prop->name  == SGFAB || SGFB)? 1 : 2;
            has_move = true;
            break;
        }
    }
    return;
}

// will return a vector of actions
// now ignoring variations and loading only the main path
void load_sgf_tree(SGFNode* head)
{
    int index, player = 0;
    bool has_move = false;
    SGFNode* next = head;
    while(next)
    {
        sgf_play_node(next, index, player,has_move);
        // append to action vector if has move
        if(has_move)
        {
            // append to action vector if has move
        }

        next = next->child;
    }
}