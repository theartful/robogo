#include "gtp.h"

// Tournament Commands
/**
 * @param   none
 * @returns {string}    score   final game score
 */
string gtp::final_score()
{
    return "15";
}

/**
 * @param   {string}                        status  Requested status
 * @returns {MultiLineList<List<Vertex>>}   stones  Stones with the requested status
 */
MultiLineList<List<Vertex>> gtp::final_status_list(string status)
{
    MultiLineList<List<Vertex>> vertecies;
    return vertecies;
}
