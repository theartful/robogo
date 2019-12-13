#include "gtp.h"
using namespace gtp;

// Tournament Commands
/**
 * @param   none
 * @returns {string}    score   final game score
 */
std::string GTPEngine::final_score()
{
    return "15";
}

/**
 * @param   {string}                        status  Requested status
 * @returns {MultiLineList<List<Vertex>>}   stones  Stones with the requested status
 */
MultiLineList<List<Vertex>> GTPEngine::final_status_list(std::string status)
{
    MultiLineList<List<Vertex>> vertecies;
    return vertecies;
}
