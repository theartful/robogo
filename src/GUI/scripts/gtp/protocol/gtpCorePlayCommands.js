// Core Play Commands
/**
 * @param   {Move}  move  a move (Color and vertex) to play
 * @returns {void}
 */
let play = (move) => {
    move = new Move(move.toString());
}

/**
 * @param   {Color}         color   Color for which to generate a move
 * @returns {Vertex|string} vertex  Vertex where the move was played or the string \resign"
 */
let genmove = (color) => {
    color = new Color(color.toString());

    let vertex = "resign";
    // try to genrate of move and if possiple assign the vertex to vertex if not pass

    return vertex;
}

/**
 * @param   none
 * @returns {void}
 */
let undo = () => {
    // undo
}
