// Regression Commands
/**
 * @param   {string}  filename    Name of an sgf file.
 * @param   {int}     move_number Optional move number.
 * @returns {void}
 */
let loadsgf = (filename, move_number) => {
    if (typeof filename !== "string")
        throw "Invalid filename";

    try {
        move_number = toInt(move_number);
    }
    catch(exception) {
        throw `Invalid move number is ${exception}`;
    }

    // load sgf file
}

/**
 * @param   none
 * @returns {Vertex|string}    Vertex    where the engine would want to play a move or the string \resign"
 */
let reg_genmove = (color) => {
    if (typeof color !== "Color")
        throw "Invalid Argument: provided color is not of type color";

    let vertex = "resign";
    // try to genrate of move and if possiple assign the vertex to vertex if not pass

    return vertex;
}