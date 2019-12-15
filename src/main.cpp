// #include "SGF_GNU_library/SGFinterface.h"
#include "GUI/agent/agent.h"
#include "GUI/server/Server.h"
#include "SimpleGUI/simplegui.h"
#include "agents/mcts_agent.h"
#include "controller/game.h"
#include "engine/board.h"
#include "gtp/gtp.h"
#include "net/game_manager.h"
#include <memory>
#include <time.h>

using namespace go::engine;
using namespace go;
using namespace go::simplegui;
using namespace go::gui;

Server* s = NULL;
void sigint_handler(int signal)
{
	if (s != NULL)
		delete s;
	exit(1);
}

std::string get_parameter(int argc, const char* argv[], std::string long_parameter, std::string short_parameter)
{
	for (int i = 0; i < argc-1; i++)
	{
		if (strcmp(argv[i], long_parameter.c_str()) == 0 || strcmp(argv[i], short_parameter.c_str()) == 0)
			return argv[i+1];
	}

	return "";
}

int main(int argc, const char* argv[])
{
	struct sigaction sigIntHandler;
	sigIntHandler.sa_handler = sigint_handler;
	sigemptyset(&sigIntHandler.sa_mask);
	sigIntHandler.sa_flags = 0;
	sigaction(SIGINT, &sigIntHandler, NULL);

	if (get_parameter(argc, argv, "--mode", "-m") == "gtp")
	{
		gtp::GTPEngine engine;
		engine.game_loop();
	}
	else if (get_parameter(argc, argv, "--mode", "-m") == "debug")
	{
		Game game;
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
		std::string port_str = get_parameter(argc, argv, "--port", "-p");
		if (port_str != "")
		{
			uint32_t port = Server::parse_port(port_str);
			if (port > 0)
				s = Server::setup(mode1, mode2, port);
			else
			{
				std::cout << "Error: invalid port" << std::endl;
				exit(1);
			}
		}
		else
			s = Server::setup(mode1, mode2);

		if (mode1 == 'r' || mode2 == 'r')
		{
			std::string uri = get_parameter(argc, argv, "--uri", "-u");
			uri = (uri != "") ? uri : "ws://localhost:8080";

			std::cout << "net_game_uri: " << uri << std::endl;
			s->net_game = true;
			try
			{
				net::GameManager GM(uri, s);
				GM.run();
			}
			catch(const std::exception& e)
			{
				std::cerr << e.what() << '\n';
			}
		}
		else
		{
			Game game;
			s->bind_game(game);

			std::shared_ptr<Agent> agent1;
			std::shared_ptr<Agent> agent2;

			if (mode1 == 'h')
				agent1 = std::make_shared<HumanAgent>(s, "black");
			else if (mode1 == 'a')
				agent1 = std::make_shared<MCTSAgent>();
			else
				throw std::invalid_argument("invalid player 1 mode");

			if (mode2 == 'h')
				agent2 = std::make_shared<HumanAgent>(s, "white");
			else if (mode2 == 'a')
				agent2 = std::make_shared<MCTSAgent>();
			else
				throw std::invalid_argument("invalid player 2 mode");

			std::cout << "running local game mode1: " << mode1
			          << ", mode2: " << mode2 << std::endl;
			agent1->set_player_idx(0);
			agent2->set_player_idx(1);
			game.register_agent(agent1, 0);
			game.register_agent(agent2, 1);
			game.main_loop();
			s->end_game(game);
			sleep(2);
		}
	}

	if (s != NULL)
		delete s;
	return 0;
}
