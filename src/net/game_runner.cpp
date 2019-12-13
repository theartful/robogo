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

void NetGameRunner::run_game(GameManager& manager, uint32_t netagent_color, std::vector<Action> init_actions, std::array<uint32_t, 2> elapsed_time)
{
    net_agent = std::make_shared<RemoteAgent>();
    auto mcts_agent = std::make_shared<MCTSAgent>();
    auto local_agent = std::make_shared<LocalAgent>(&manager, mcts_agent);
    game.register_agent(net_agent, netagent_color);
    game.register_agent(local_agent, 1 - netagent_color);
    game.force_moves(init_actions);
    for (uint32_t i = 0; i < 2; i++)
    {
        std::cout << elapsed_time[i] << "\n";
        auto elapsed_time_duration = std::chrono::duration<uint32_t, std::milli>(elapsed_time[i]);
        game.set_elapsed_time(elapsed_time_duration, i);
    }
    game.main_loop();
    manager.runner_lifetime_over();
    local_agent->set_game_end(true);
}

void NetGameRunner::set_game_end(bool game_end) 
{
    game.set_game_end(true);
}

void NetGameRunner::set_remote_move(const ActionMessage message) 
{
    DEBUG_PRINT("NetGameRunner: Setting remote move..\n");
    net_agent->set_player_pos(message.action.pos);
    DEBUG_PRINT("NetGameRunner: Notification sent..\n");
}

void NetGameRunner::set_remaining_time(std::chrono::duration<uint32_t, std::milli> remaining_time, uint32_t agent_id)
{
    game.set_remaining_time(remaining_time, agent_id);
}


}

}