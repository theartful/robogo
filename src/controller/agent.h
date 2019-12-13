#ifndef SRC_CONTROLLER_AGENT_H
#define SRC_CONTROLLER_AGENT_H

#include "engine/board.h"

namespace go
{

class Game;

class Agent
{
public:
	virtual ~Agent(){};
	virtual uint32_t generate_move(const Game& game) = 0;
	uint32_t get_player_idx() const
	{
		return player_idx;
	}
	virtual void set_player_idx(uint32_t player_idx_)
	{
		this->player_idx = player_idx_;
	}

private:
	uint32_t player_idx;
};

} // namespace go

#endif // SRC_CONTROLLER_AGENT_H
