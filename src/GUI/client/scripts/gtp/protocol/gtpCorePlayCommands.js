// Core Play Commands
/**
 * @param   {Move}  move  a move (Color and vertex) to play
 * @returns {void}
 */
let play = (move) => {
    move = new Move(move.toString());
    let color = new Color(move.toString().split(' ')[0]).toString();
    let indecies = Vertex.indecies(new Vertex(move.toString().split(' ')[1]));
    let row = arraySize - (indecies.row - 1);
    let column = indecies.column - 1;
    addPiece(column, row, color);
}

let genmoveId = 0;

/**
 * @param   {Color}         color   Color for which to generate a move
 * @returns {Vertex|string} vertex  Vertex where the move was played or the string \resign"
 */
let genmove = (c) => {
    let color = new Color(c.toString());
    currentPlayer = color.toString();
    allowMove = true;
    return "break";
}

pieceLocation.watch('location', (id, oldval, newval) => {
    allowMove = false;
    if (newval.length !== 2)
        socket.send(`=${genmoveId} resign\n\n`);
    
    let row = (arraySize - (newval[1] - 1)).toString();
    row = (row.length == 1) ? `0${row}` : row;
    let column = (newval[0] + 1 < 9) ? newval[0] + 97 : newval[0] + 98;

    let columnLetter = String.fromCharCode(column);
    let vertex = new Vertex(`${columnLetter}${row}`);
    socket.send(`=${genmoveId} ${vertex.toString()}\n\n`);
});

/**
 * @param   none
 * @returns {void}
 */
let undo = () => {
    // undo
}
