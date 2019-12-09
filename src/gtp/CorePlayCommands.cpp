#include "gtp.h"

// Core Play Commands

/**
 * @param   {Move}  move  a move (Color and vertex) to play
 * @returns {void}
 */
void gtp::play(Move move)
{

}

/**
 * @param   {Color}         color   Color for which to generate a move
 * @returns {Vertex|string} vertex  Vertex where the move was played or the string \resign"
 */
Alternative<Vertex, string> gtp::genmove(Color color)
{
    return Alternative<Vertex, string>("pass");
}

/**
 * @param   none
 * @returns {void}
 */
void gtp::undo()
{

}
