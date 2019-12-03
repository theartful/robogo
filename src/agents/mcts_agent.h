#ifndef SRC_AGENTS_MCTS_AGENT_H
#define SRC_AGENTS_MCTS_AGENT_H

#include "controller/agent.h"
#include "controller/game.h"
#include "engine/board.h"
#include "mcts/mcts.h"

namespace go
{

class MCTSAgent : public Agent
{
public:
	uint32_t generate_move(const Game& game) override
	{
		mcts_algo.clear_tree();
		return mcts_algo.run(game.get_game_state()).pos;
	}

private:
	mcts::MCTS mcts_algo;
};

} // namespace go

#endif // SRC_AGENTS_MCTS_AGENT_H
