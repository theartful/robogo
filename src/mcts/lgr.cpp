#include <array>
#include "mcts/lgr.h"
#include "engine/board.h"

using namespace go::lgr;
using namespace go::engine;

//clear stored values in lgr
void LGR::clear(){
    is_set_lgr1[0].fill(false);
    is_set_lgr1[1].fill(false);

    is_set_lgr2[0].fill(false);
    is_set_lgr2[1].fill(false);
}

//checks if last good reply of action is stored in lgr-1
bool LGR::is_stored(const engine::Action& action){
    return is_set_lgr1[action.pos][1-action.player_index];
}

//checks if last good reply of action is stored in lgr-2
bool LGR::is_stored(const Action& before_prev,const Action& prev){
    return is_set_lgr2[before_prev.pos+BoardState::MAX_NUM_CELLS*prev.pos][before_prev.player_index];
}

//returns last good reply for an action in lgr-1
Action LGR::get_lgr(const Action& action){
    return lgr1[action.pos][1-action.player_index];
}

//returns last good reply for an action in lgr-2
Action LGR::get_lgr(const Action& before_prev, const Action& prev){
    return lgr2[before_prev.pos+BoardState::MAX_NUM_CELLS*prev.pos][before_prev.player_index];
}

//update last good reply for an action in lgr-1
void LGR::set_lgr(const Action& prev, const engine::Action& best){
    lgr1[prev.pos][best.player_index] = best;
    is_set_lgr1[prev.pos][best.player_index] = true;
}

//update last good reply - 2
void LGR::set_lgr(const Action& before_prev, const Action& prev, const Action& best){
    lgr2[before_prev.pos+BoardState::MAX_NUM_CELLS*prev.pos][best.player_index] = best;
    is_set_lgr2[before_prev.pos+BoardState::MAX_NUM_CELLS*prev.pos][best.player_index] = true;
}

//remove last good reply of an action
void LGR::remove_lgr(const Action& action){
    is_set_lgr1[action.pos][1-action.player_index] = false;
}