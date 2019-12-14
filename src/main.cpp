// #include "SGF_GNU_library/SGFinterface.h"
#include "SimpleGUI/simplegui.h"
#include "agents/mcts_agent.h"
#include "controller/game.h"
#include "engine/board.h"
#include "GUI/server/Server.h"
#include "GUI/agent/agent.h"
#include "gtp/gtp.h"
#include "net/game_manager.h"
#include <memory>
#include <time.h>

using namespace go::engine;
using namespace go;
using namespace go::simplegui;
using namespace go::gui;

int main(int argc, const char * argv[])
{
	if (argc > 1 && strcmp(argv[1], "gtp") == 0)
	{
		gtp::GTPEngine engine;
		engine.game_loop();
	}
	else if (argc > 1 && strcmp(argv[1], "tournament") == 0)
	{
		net::GameManager GM("ws://localhost:8080");
		GM.run();
	}
	else
	{
		Game game;
		if (argc > 1 && strcmp(argv[1], "debug") == 0)
		{
			auto agent1 = std::make_shared<BoardSimpleGUI>();
			auto agent2 = std::make_shared<MCTSAgent>();
			agent1->set_player_idx(0);
			agent2->set_player_idx(1);
			game.register_agent(agent1, 0);
			game.register_agent(agent2, 1);
			game.main_loop();
		}
		else
		{
			char mode1 = 'a';
			char mode2 = 'a';

			Server* s = Server::setup(game, mode1, mode2);

			std::shared_ptr<Agent> agent1;
			std::shared_ptr<Agent> agent2;

			if (mode1 == 'h')
				agent1 = std::make_shared<HumanAgent>(s, "black");
			else if (mode1 == 'a')
				agent1 = std::make_shared<MCTSAgent>();
			else if (mode1 == 'r')
				agent1 = std::make_shared<MCTSAgent>();
			else
				throw std::invalid_argument("invalid player 1 mode");

			if (mode2 == 'h')
				agent2 = std::make_shared<HumanAgent>(s, "white");
			else if (mode2 == 'a')
				agent2 = std::make_shared<MCTSAgent>();
			else if (mode1 == 'r')
				agent2 = std::make_shared<MCTSAgent>();
			else
				throw std::invalid_argument("invalid player 2 mode");
			
			agent1->set_player_idx(0);
			agent2->set_player_idx(1);
			game.register_agent(agent1, 0);
			game.register_agent(agent2, 1);
			game.main_loop();
			s->end_game(game);
			sleep(2);
		}
	}

	return 0;
}
