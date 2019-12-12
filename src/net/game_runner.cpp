#include "game_runner.h"
#include <atomic>
#include <thread>

using namespace go;
using namespace go::net;

namespace go
{

namespace net
{


NetGameRunner::NetGameRunner()
{

}

void NetGameRunner::run_game(uint32_t netagent_color, std::vector<Action> init_actions)
{
    auto net_agent = std::make_shared<RemoteAgent>();
    auto mcts_agent = std::make_shared<MCTSAgent>();
    game.register_agent(net_agent, netagent_color);
    game.register_agent(mcts_agent, 1 - netagent_color);
    game.force_moves(init_actions);
    game.main_loop();
}

void NetGameRunner::set_game_end(bool game_end) 
{
    game.set_game_end(true);
}
        
void NetGameRunner::set_remote_move(const ActionMessage message) {
    DEBUG_PRINT("NetGameRunner: Setting remote move..\n");
    std::lock_guard<std::mutex> lock(remote_move_mutex);
    // net_agent->set_player_pos(message.action.pos);
    remote_move_arrival.notify_all();
    DEBUG_PRINT("NetGameRunner: Notification sent..\n");
}


}

}