#ifndef NET_ACTION_MESSAGE_H
#define NET_ACTION_MESSAGE_H

#include <atomic>
#include <utility>
#include "controller/game.h"
#include <mutex>
#include <condition_variable>


using go::engine::Action;

namespace go
{

namespace net
{

// TODO: make these into locals of a struct with one instance.
extern std::mutex remote_move_mutex;
extern std::condition_variable remote_move_arrival;

extern bool player_pos_arrived;
extern uint32_t net_player_pos; 

// TODO: remove ActionMessage.
struct ActionMessage
{
    bool taken;
    Action action;
};

using atomic_message = std::atomic<ActionMessage>;


}

}

#endif // NET_ACTION_MESSAGE_H