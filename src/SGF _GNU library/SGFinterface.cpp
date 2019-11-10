#include "SGFinterface.h"
// #include "../engine/board.h"
#define T  84

void get_sgf_move(SGFProperty* prop, int& x, int& y, bool& isPass)
{
    int length = strlen(prop->value);

    if(length < 2)
    {
       if(prop->value[0] == '\0')
       {
           printf("it's a pass move\n");
           isPass = true;
       }
       else
       {
           perror("wrong format\n");
       }
          
    }

    if(x == T && y == T)
    {
        //it's a pass too
        isPass = true;
        return;
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
    bool isPass;
    for (prop = node->props; prop; prop = prop->next)
    {
        switch (prop->name)
        {
            case SGFB:
            case SGFW: 
            case SGFAB:
            case SGFAW:
            x,y = 0;
            get_sgf_move(prop, x, y, isPass);
            if(isPass)
            {
                index = BoardState::INVALID_INDEX;
                return;
            }
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
std::vector<Action> load_sgf_tree(SGFNode* head)
{
    std::vector<Action> moves;

    int index, player = 0;
    bool has_move = false;
    SGFNode* next = head;

    while(next)
    {
        sgf_play_node(next, index, player,has_move);
        // append to action vector if has move
        if(has_move)
        {
            Action move;
            move.player_index = player;
            move.pos = index;
            
            moves.push_back(move);
        }

        next = next->child;
    }

    return moves;
}

//-------------------------------------write functions-----------------------------------------------------//
// assuming the input is a vector of moves that can either be a pass or add black (white res)


void get_move_x_y(uint32_t index, char* x, char* y)
{
    *x = (index / BoardState::MAX_BOARD_SIZE) + 98;
    *y = (index % BoardState::MAX_BOARD_SIZE) + 98;
}

void extract_sgf_file(std::vector<Action>& moves, const char* file_name)
{
    SGFNode* head = sgfNewNode() ;
    char* x;
    char* y;
    sgf_write_header_reduced(head, 0);

    for (int i=0; i < moves.size(); i++) 
    {
        if(moves[i].pos > BoardState::INVALID_INDEX)
        {
            printf("wrong entry in pos\n");
            return;
        }

        SGFNode* temp = sgfAddChild(temp);
        get_move_x_y(moves[i].pos, x, y);
        strcat(x,y);

        const char* name = (moves[i].player_index == 1) ? "AB" : "AW";
        sgfAddProperty(temp,name, x);
        
    }

    writesgf(head, file_name);
}