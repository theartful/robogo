#include "mcts/prior_knowledge.h"

namespace go
{
namespace mcts
{

PriorKnowledge::PriorKnowledge(PlayoutPolicy& playout_policy_)
    : playout_policy{playout_policy_}
{
}

} // namespace mcts
} // namespace go