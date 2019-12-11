#include "SimpleGUI/simplegui.h"
#include "controller/game.h"
#include "engine/board.h"
#include "GUI/server/Server.h"
#include "GUI/agent/agent.h"
#include <memory>

using namespace go::engine;
using namespace go;
using namespace go::simplegui;
using namespace go::gui;

int main(int argc, const char * argv[])
{
	char mode1 = 'a';
	char mode2 = 'a';
	if (argc >= 2)
	{
		string m1 = argv[1];
		if (!(m1 == "h" || m1 == "H" || m1 == "a" || m1 == "A"))
		{
			std::cout << "invalid arguments\n";
			exit(-1);
		}
		mode1 = m1[0];
	}

	if (argc >= 3)
	{
		string m2 = argv[2];
		if (!(m2 == "h" || m2 == "H" || m2 == "a" || m2 == "A"))
		{
			std::cout << "invalid arguments\n";
			exit(-1);
		}
		mode2 = m2[0];
	}

	Game game;
	Server* s = Server::setup(game);

	std::shared_ptr<Agent> agent1;
	std::shared_ptr<Agent> agent2;

	if (mode1 == 'h')
		agent1 = std::make_shared<HumanAgent>(s, "black");
	else if (mode1 == 'a')
		agent1 = std::make_shared<BoardSimpleGUI>();
	else if (mode1 == 'r')
		agent1 = std::make_shared<BoardSimpleGUI>();
	else
		throw std::invalid_argument("invalid player 1 mode");

	if (mode2 == 'h')
		agent2 = std::make_shared<HumanAgent>(s, "white");
	else if (mode2 == 'a')
		agent2 = std::make_shared<BoardSimpleGUI>();
	else if (mode1 == 'r')
		agent2 = std::make_shared<BoardSimpleGUI>();
	else
		throw std::invalid_argument("invalid player 2 mode");
	
	agent1->set_player_idx(0);
	agent2->set_player_idx(1);
	game.register_agent(agent1, 0);
	game.register_agent(agent2, 1);
	game.main_loop();
}
