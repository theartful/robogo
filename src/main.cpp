#include "SimpleGUI/simplegui.h"
#include "controller/game.h"
#include "engine/board.h"
#include "GUI/server/Server.h"
#include "GUI/agent/gui.h"
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
	Agent* simplegui_agent1 = (mode1 == 'a' || mode1 == 'A') ? new BoardSimpleGUI() : NULL;
	Agent* simplegui_agent2 = (mode2 == 'a' || mode2 == 'A') ? new BoardSimpleGUI() : NULL;

	auto agent1 = std::make_shared<BoardGUI>(s, mode1, "black", simplegui_agent1);
	auto agent2 = std::make_shared<BoardGUI>(s, mode2, "white", simplegui_agent2);

	agent1->set_player_idx(0);
	agent2->set_player_idx(1);
	game.register_agent(agent1, 0);
	game.register_agent(agent2, 1);
	game.main_loop();
}
