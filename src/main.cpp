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

int main()
{
	Game game;
	Server* s = Server::setup();
	Agent* simplegui_agent1 = new BoardSimpleGUI();
	Agent* simplegui_agent2 = new BoardSimpleGUI();
	auto agent = std::make_shared<BoardGUI>(s, 'a', "white", simplegui_agent1);
	auto agent2 = std::make_shared<BoardGUI>(s, 'a', "black", simplegui_agent2);
	agent->set_player_idx(0);
	agent2->set_player_idx(1);
	game.register_agent(agent, 0);
	game.register_agent(agent2, 1);
	game.main_loop();
}
