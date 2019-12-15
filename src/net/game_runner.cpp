#include "game_runner.h"
#include <atomic>
#include <thread>
#include "local_agent.h"
using namespace go;
using namespace go::net;

namespace go
{

namespace net
{


NetGameRunner::NetGameRunner()
{

}

void NetGameRunner::run_game(GameManager& manager, uint32_t netagent_color, std::vector<Action> init_actions)
{
    net_agent = std::make_shared<RemoteAgent>();
    auto mcts_agent = std::make_shared<MCTSAgent>();
    auto local_agent = std::make_shared<LocalAgent>(&manager, mcts_agent);
    game.register_agent(net_agent, netagent_color);
    game.register_agent(local_agent, 1 - netagent_color);
    game.force_moves(init_actions);
    game.main_loop();
    manager.runner_lifetime_over();
}

void NetGameRunner::set_game_end(bool game_end) 
{
    game.set_game_end(true);
}
        
void NetGameRunner::set_remote_move(const ActionMessage message) {
    DEBUG_PRINT("NetGameRunner: Setting remote move..\n");
    net_agent->set_player_pos(message.action.pos);
    DEBUG_PRINT("NetGameRunner: Notification sent..\n");
}

void NetGameRunner::bind_gui(Server* s)
{
    s->bind_game(game);
}

Game& NetGameRunner::get_game()
{
    return this->game;
}

}

}