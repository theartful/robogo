#include "SGFinterface.h"

Action sgf_play_node(SGFNode *node)
{

    SGFProperty *prop;
    for (prop = node->props; prop; prop = prop->next)
    {
        switch (prop->name)
        {
            case SGFAB: // case add black to the board
        }
    }
}