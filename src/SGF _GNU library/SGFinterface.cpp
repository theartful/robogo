#include "SGFinterface.h"

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

void sgf_play_node(SGFNode *node, uint32_t& index, uint32_t& player, bool& has_move, bool& isPass)
{
    SGFProperty *prop = node->props;
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
            player = (prop->name  == SGFB)? 0 : 1;
            get_sgf_move(prop, x, y, isPass);
            if(isPass)
            {
                // index = go::engine::BoardState::INVALID_INDEX;
                isPass = true;
                return;
            }
            index = go::engine::BoardState::index(static_cast<uint32_t> (y),static_cast<uint32_t> (x));
            has_move = true;
            break;
            default:
            index = go::engine::BoardState::INVALID_INDEX;
            printf("wrong format for attribute\n");
            
        }
    }
    return;
}

// will return a vector of actions
// now ignoring variations and loading only the main path
std::vector<go::engine::Action> load_sgf_tree(SGFNode* head)
{
    std::vector<go::engine::Action> moves;

    uint32_t index, player = 0;
    bool has_move, isPass = false;
    SGFNode* next = head;

    while(next)
    {
        sgf_play_node(next, index, player,has_move, isPass);
        if(index ==  go::engine::BoardState::INVALID_INDEX)
        {
            next = next->child;
            continue;
        }
        // append to action vector if has move
        if(has_move)
        {
            go::engine::Action move;
            move.player_index = static_cast<uint32_t>(player);
            move.pos = static_cast<uint32_t>(index);
            if(isPass)
            {
                move.pos =  go::engine::Action::PASS;
            }
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
    *x = (index / go::engine::BoardState::MAX_BOARD_SIZE) + 98;
    *y = (index % go::engine::BoardState::MAX_BOARD_SIZE) + 98;
}

void extract_sgf_file(std::vector<go::engine::Action>& moves, const char* file_name)
{
    SGFNode* head = sgfNewNode();
    char* x = NULL;
    char* y = NULL;
    sgf_write_header(head,0,0,0,0,0,0);

    for (uint32_t i=0; i < moves.size(); i++) 
    {
        if(moves[i].pos > go::engine::BoardState::INVALID_INDEX)
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


// int main()
// {
//   char const* filename = "ko.sgf"; 
//   SGFNode *treeHead = readsgffile(filename);
//   printGameTree(treeHead);
//   std::vector<go::engine::Action> moves = load_sgf_tree(treeHead);
//   printGameTree(treeHead);
//   sgfFreeNode(treeHead);

// }