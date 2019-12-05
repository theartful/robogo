#ifndef LGR_H
#define LGR_H

#include <array>
#include "engine/board.h"

using namespace go::engine;

namespace go
{
namespace lgr{

class LGR{

public:

    LGR(){clear();}

    LGR(const LGR& other){ *this = other; }

    //clear stored values in lgr
    void clear();

    //checks if last good reply of action is stored in lgr-1
    bool is_stored(const Action& action);

    //checks if last good reply of action is stored in lgr-2
    bool is_stored(const Action& before_prev,const Action& prev);

    //returns last good reply for an action in lgr-1
    Action get_lgr(const Action& action);

    //returns last good reply for an action in lgr-2
    Action get_lgr(const Action& before_prev, const Action& prev);

    //update last good reply - 1
    void set_lgr(const Action& prev, const Action& best);

    //update last good reply - 2
    void set_lgr(const Action& before_prev, const Action& prev, const Action& best);

    //remove last good reply of an action
    void remove_lgr(const Action& action);

private:
    static constexpr int32_t UNSET_VALUE = -1;


    //I will extend this incrementely to (lgr2,lgrf_1) and maybe (lgrf_2)
    //lgr1[action][i] -> last good reply for player i where other played action
    std::array<std::array<Action,BoardState::MAX_NUM_CELLS>,2> lgr1; 

    std::array<std::array<Action,BoardState::MAX_NUM_CELLS*BoardState::MAX_NUM_CELLS>,2> lgr2; 

    std::array<std::array<bool,BoardState::MAX_NUM_CELLS>,2> is_set_lgr1; 

    std::array<std::array<bool,BoardState::MAX_NUM_CELLS*BoardState::MAX_NUM_CELLS>,2> is_set_lgr2; 
};

}
}






#endif