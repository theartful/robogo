#include "includes/catch.hpp"


#include "controller/agent.h"
#include "controller/game.h"
#include "engine/board.h"
#include "engine/cluster.h"

using namespace go;
using namespace go::engine;



TEST_CASE("tryTest")
{
	REQUIRE(1 == 1);
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
