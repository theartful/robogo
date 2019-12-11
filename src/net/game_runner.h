#ifndef NET_GAME_RUNNER_H
#define NET_GAME_RUNNER_H

#include "game_manager.h"

#include "../SimpleGUI/simplegui.h"
#include "../agents/mcts_agent.h"
#include "controller/game.h"

#include <atomic>
#include <utility>

using namespace go;
using namespace go::engine;

namespace go
{

namespace net
{

struct ActionMessage
{
    bool taken;
    Action action;
};


class NetGameRunner
{
    public:
        NetGameRunner() 
        {
            force_end = false;
        }

        void run_game(uint32_t netagent_color, std::vector<Action> init_actions)
        {
            Game game (&force_end);
            auto net_agent = std::make_shared<go:: simplegui::BoardSimpleGUI>();
            auto mcts_agent = std::make_shared<MCTSAgent>();
            net_agent->set_player_idx(netagent_color);
            mcts_agent->set_player_idx(1 - netagent_color);
            game.register_agent(net_agent, netagent_color);
            game.register_agent(mcts_agent, 1 - netagent_color);
            game.force_moves(init_actions);
            game.main_loop();
        }

        void set_game_end(bool game_end) {
            force_end = game_end;
        }

        void set_remote_move(const ActionMessage message) {
            remote_move = message;
        }

    private:
        std::atomic_bool force_end;
        std::atomic<ActionMessage> remote_move;
        std::atomic<ActionMessage> local_move;
};

}

}

#endif // NET_GAME_RUNNER_H