#include "gtp.h"
#include "entities.h"

// Tournament Commands

/**
 * @param   {int}   main_time       Main time measured in seconds
 * @param   {int}   byo_yomi_time   Byo yomi time measured in seconds
 * @param   {int}   byo_yomi_stones Number of stones per byo yomi period
 * @returns {void}
 */
void time_settings(uint32_t main_time, uint32_t byo_yomi_time, uint32_t byo_yomi_stones)
{

}

/**
 * @param   {Color} color   Color for which the information applies
 * @param   {int}   time    Number of seconds remaining
 * @param   {int}   stones  Number of stones remaining
 * @returns {void}
 */
void time_left(Color color, uint32_t time, uint32_t stones)
{

}

/**
 * @param   none
 * @returns {string}    score   final game score
 */
string final_score()
{

}

/**
 * @param   {string}                        status  Requested status
 * @returns {MultiLineList<List<Vertex>>}   stones  Stones with the requested status
 */
MultiLineList<List<Vertex>> final_status_list(string status)
{

}
