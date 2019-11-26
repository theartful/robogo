#include "includes/catch.hpp"


#include "controller/agent.h"
#include "controller/game.h"
#include "engine/board.h"
#include "engine/cluster.h"
#include "SGF _GNU library/SGFinterface.h"

using namespace go;
using namespace go::engine;



TEST_CASE("test sgf")
{
    Game game;
    bool all = true;
    int k = 0;  
    char const* filename = "ko.sgf"; 
    SGFNode *treeHead = readsgffile(filename);
    std::vector<go::engine::Action> actions = load_sgf_tree(treeHead);
    REQUIRE (actions.size() == 2 );
    for (unsigned int i = 0; i < actions.size();i++)
    {
        k ++;
        if (i == 6) 
            all = all && !(game.make_move(actions[i]));
        else
            all = all && game.make_move(actions[i]);
    }
    REQUIRE( k == 2);
    REQUIRE (all == true);
}



TEST_CASE("Board updates")
{
	Game game;
	BoardState state;

    SECTION("checking correct placement of cell"){
        uint32_t place = BoardState::index(0, 2);
        state.board[place] = Cell::WHITE; //unit functionaality assumed correct
        Action action = {place,1};
        REQUIRE(game.make_move(action)==1);
        REQUIRE(game.get_board_state().board[place] == state.board[place] );
    }

	
}
