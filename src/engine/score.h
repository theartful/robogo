#ifndef _ENGINE_SCORE_H_
#define _ENGINE_SCORE_H_

#include "game.h"

namespace go::engine
{

std::pair<float, float>
calculate_score(const GameState& state, const Rules& rules);

}

#endif
