#ifndef LOCAL_AGENT_H
#define LOCAL_AGENT_H

#include "controller/agent.h"
#include "game_manager.h"
#include "engine/board.h"
#include "action_message.h"

using namespace go;
using namespace go::engine;

namespace go
{

namespace net
{

class LocalAgent : public Agent
{
public:
    LocalAgent(GameManager* manager, std::shared_ptr<Agent> wrapped_agent)
    {
        game_manager = manager;
        agent = std::move(wrapped_agent);
    }

    virtual uint32_t generate_move(const Game& game) override
    {
        if (game_end)
            return 0;
        DEBUG_PRINT("Generating Agent's move..\n");
        uint32_t move = agent->generate_move(game);
        DEBUG_PRINT("Sending Agent's move..\n");
        Action action;
        action.player_index = agent->get_player_idx();
        action.pos = move;
        game_manager->send_move(action);
        return move;
    }

    virtual void set_player_idx(uint32_t player_idx_) override
    {
        Agent::set_player_idx(player_idx_);
        agent->set_player_idx(player_idx_);
    }

    void set_game_end(bool game_finished)
    {
        game_end = game_finished;
    }

private:
    bool game_end = false;
    GameManager* game_manager;
    std::shared_ptr<Agent> agent;
};

}

}



#endif // LOCAL_AGENT_H