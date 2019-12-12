#ifndef NET_AGENT_H
#define NET_AGENT_H

#include "controller/agent.h"
#include "engine/board.h"

#include <atomic>
#include <memory>
#include <thread>
#include <chrono>
#include <mutex>
#include <condition_variable>

#include "action_message.h"

using namespace go;
using namespace go::engine;

namespace go
{

namespace net
{

class RemoteAgent : public Agent
{
public:
    virtual uint32_t generate_move(const Game& game) override
    {
        DEBUG_PRINT("RemoteAgent: Going to wait..\n");
        std::unique_lock<std::mutex> lock(remote_move_mutex);
        remote_move_arrival.wait(lock, [this]{
            return player_pos_arrived; } );
        DEBUG_PRINT("RemoteAgent: I have finished waiting..\n");
        player_pos_arrived = false;
        return net_player_pos;
    }

    void set_player_pos (uint32_t pos)
    {
        player_pos_arrived = true;
        net_player_pos = pos;
    }
};
    


}

}

#endif // NET_AGENT_H