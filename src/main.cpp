// #include "SGF_GNU_library/SGFinterface.h"
#include "SimpleGUI/simplegui.h"
#include "agents/mcts_agent.h"
#include "controller/game.h"
#include "engine/board.h"
#include "net/net_game_manager.h"
#include <memory>

using namespace go::engine;
using namespace go;
using namespace go::simplegui;

int main()
{
	net::NetGameManager GM("ws://localhost:8080");
	GM.run();
	return 0;
}
