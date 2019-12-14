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