#include "gtp.h"

// Regression Commands

/**
 * @param   {string}  filename    Name of an sgf file.
 * @param   {int}     move_number Optional move number.
 * @returns {void}
 */
void gtp::loadsgf(string filename, uint32_t move_number)
{

}

/**
 * @param   none
 * @returns {Vertex|string}    Vertex    where the engine would want to play a move or the string \resign"
 */
Alternative<Vertex, string> gtp::reg_genmove(Color color)
{
    return Alternative<Vertex, string>("pass");
}
