#include "controller/game.h"
#include "engine/board.h"
#include "gtp/gtp.h"
#include <memory>
#include <iostream>

using namespace go::engine;
using namespace go;

int main()
{
	go::gtp::GTPController controller;
	controller.main_loop();
	/*
	Game game;
	auto agent = std::make_shared<BoardSimpleGUI>();
	auto agent2 = std::make_shared<BoardSimpleGUI>();
	agent->set_player_idx(0);
	agent2->set_player_idx(1);
	game.register_agent(agent, 0);
	game.register_agent(agent2, 1);
	game.main_loop();
	*/
}
