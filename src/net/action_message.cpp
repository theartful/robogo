#include "action_message.h"


namespace go
{

namespace net
{

bool player_pos_arrived = false;
uint32_t net_player_pos; 
std::mutex remote_move_mutex;
std::condition_variable remote_move_arrival;

}

}