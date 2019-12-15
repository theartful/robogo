// #include "SGF_GNU_library/SGFinterface.h"
#include "SimpleGUI/simplegui.h"
#include "agents/mcts_agent.h"
#include "controller/game.h"
#include "engine/board.h"
#include <memory>
#include "config.h"
#include "mcts/pattern3x3.h"

using namespace go::engine;
using namespace go;
using namespace go::simplegui;

int main()
{
	mcts::load_patterns_file(PATTERNFILE);

	Game game;
	auto agent = std::make_shared<BoardSimpleGUI>();
	auto agent2 = std::make_shared<MCTSAgent>();
	agent->set_player_idx(0);
	agent2->set_player_idx(1);
	game.register_agent(agent, 0);
	game.register_agent(agent2, 1);
	game.main_loop();
}
