#include "mcts/lgr.h"
#include "engine/board.h"
#include <array>

using namespace go::lgr;
using namespace go::engine;

// clear stored values in lgr
void LGR::clear()
{
	lgr1[0].fill(Action::INVALID_ACTION);
	lgr1[1].fill(Action::INVALID_ACTION);
	// is_set_lgr2[0].fill(false);
	// is_set_lgr2[1].fill(false);
}

// checks if last good reply of action is stored in lgr-1
bool LGR::is_stored(const engine::Action& action) const
{
	if (is_invalid(action) || is_pass(action))
		return false;

	return lgr1[action.pos][1 - action.player_index] != Action::INVALID_ACTION;
}

// checks if last good reply of action is stored in lgr-2
bool LGR::is_stored(const Action& before_prev, const Action& prev) const
{
	return true;
	/*
	if (is_invalid(before_prev) || is_pass(before_prev))
	    return false;
	if (is_invalid(prev) || is_pass(prev))
	    return false;

	return is_set_lgr2[before_prev.pos + BoardState::MAX_NUM_CELLS * prev.pos]
	                  [before_prev.player_index];
	*/
}

// returns last good reply for an action in lgr-1
Action LGR::get_lgr(const Action& action) const
{
	return {lgr1[1 - action.player_index][action.pos], 1 - action.player_index};
}

// returns last good reply for an action in lgr-2
Action LGR::get_lgr(const Action& before_prev, const Action& prev) const
{
	return Action();
	// return lgr2[before_prev.pos + BoardState::MAX_NUM_CELLS * prev.pos]
	//           [before_prev.player_index];
}

// update last good reply for an action in lgr-1
void LGR::set_lgr(const Action& prev, const engine::Action& best)
{
	if (is_invalid(prev) || is_pass(prev))
		return;

	if (is_invalid(best) || is_pass(best))
		return;

	lgr1[best.player_index][prev.pos] = best.pos;
}

// update last good reply - 2
void LGR::set_lgr(
    const Action& before_prev, const Action& prev, const Action& best)
{
	if (is_invalid(before_prev) || is_pass(before_prev))
		return;

	if (is_invalid(prev) || is_pass(prev))
		return;

	if (is_invalid(best) || is_pass(best))
		return;

	//	lgr2[before_prev.pos + BoardState::MAX_NUM_CELLS * prev.pos]
	//	    [best.player_index] = best;
	//	is_set_lgr2[before_prev.pos + BoardState::MAX_NUM_CELLS * prev.pos]
	//	           [best.player_index] = true;
}

// remove last good reply of an action
void LGR::remove_lgr(const Action& action)
{
	if (is_invalid(action) || is_pass(action))
		return;
	lgr1[1 - action.player_index][action.pos] = Action::INVALID_ACTION;
}