// Setup Commands
/**
 * @param   {int}   size    New size of the board
 * @returns {void}
 */
let boardsize = (size) => {
    try {
        size = toInt(size);
    }
    catch(exception) {
        throw `Invalid board size is ${exception}`;
    }

    if (size > 25)
        throw "Invalid board size protocol doesn't support boards bigger than 25x25";
    // set board size to new size
}

/**
 * @param   none
 * @returns {void}
 */
let clear_board = () => {
    // clear board
}

/**
 * @param   {float}    new_komi    new komi value
 * @returns {void}
 */
let komi = (new_komi) => {
    try {
        new_komi = toFloat(new_komi);
    }
    catch(exception) {
        throw `Invalid Komi is ${exception}`;
    }

    // set komi to new komi value
}

/**
 * @param   {int}           number_of_stones    Number of handicap stones
 * @returns {List<Vertex>}  vertices            A list of the vertices where handicap stones have been placed
 */
let fixed_handicap = (number_of_stones) => {
    try {
        number_of_stones = toInt(number_of_stones);
    }
    catch(exception) {
        throw `Invalid number of stones is ${exception}`;
    }

    let vertices = List("Vertex");
    // place fixed handicaps
    return vertices;
}

/**
 * @param   {int}           number_of_stones    Number of handicap stones
 * @returns {List<Vertex>}  vertices            A list of the vertices where handicap stones have been placed
 */
let place_free_handicap = (number_of_stones) => {
    try {
        number_of_stones = toInt(number_of_stones);
    }
    catch(exception) {
        throw `Invalid number of stones is ${exception}`;
    }

    let vertices = List("Vertex");
    //  place free handicaps
    return vertices;
}

/**
 * @param   {List<Vertex>}  vertices  A list of vertices where handicap stones should be placed on the board
 * @returns {void}
 */
let set_free_handicap = (vertices) => {
    if (!List.prototype.isPrototypeOf(vertices) || vertices.type !== "Vertex")
        throw "Invalid List of vertices";

    // set free handicaps
}
