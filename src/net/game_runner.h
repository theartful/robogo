#ifndef NET_GAME_RUNNER_H
#define NET_GAME_RUNNER_H

#include "SimpleGUI/simplegui.h"
#include "agents/mcts_agent.h"
#include "controller/game.h"
#include "GUI/server/Server.h"

#include "action_message.h"
#include "remote_agent.h"

#include <atomic>
#include <utility>
#include <mutex>
#include <condition_variable>

using namespace go;
using namespace go::engine;

namespace go
{

namespace net
{

class GameManager;

class NetGameRunner
{
    public:
        NetGameRunner();
        void run_game(GameManager& manager, uint32_t netagent_color, std::vector<Action> init_actions);
        void set_game_end(bool game_end);
        void set_remote_move(const ActionMessage message);
        void bind_gui(Server* s);
    private:
        std::shared_ptr<RemoteAgent> net_agent;
        std::atomic_bool force_end;
        Game game;
};

}

}

#endif // NET_GAME_RUNNER_H