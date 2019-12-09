#ifndef SRC_MCTS_PRIOR_KNOWLEDGE_H
#define SRC_MCTS_PRIOR_KNOWLEDGE_H

#include "engine/board.h"
#include "mcts/mcts.h"
#include "mcts/playout.h"

namespace go
{
namespace mcts
{

class PriorKnowledge
{
public:
	PriorKnowledge(PlayoutPolicy&);
	void process_position(const engine::GameState&, std::vector<Edge>&);

private:
	PlayoutPolicy& playout_policy;
};

} // namespace mcts
} // namespace go

#endif // SRC_MCTS_PRIOR_KNOWLEDGE_H