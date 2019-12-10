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

/**
 * @param   {Board}  board  a board (Color and vertex) to play
 * @returns {void}
 */
let setboard = (newBoard) => {
    let tempBoard = new Array(19)
    .fill(null)
    .map(() => new Array(19).fill(null));

    for (let i = 0; i < 19; i++) {
        for (let j = 0; j < 19; j++) {
            let color = (newBoard[i*19+j] === ".") ? null : newBoard[i*19+j];
            tempBoard[j][i] = { color: color, selected: false };
        }
    }

    if (matchBoards(tempBoard, board)) {
        alert("invalid move!!!");
        return;
    }

    board = tempBoard;
    changePlayer(currentPlayer);
    currentPlayer = (currentPlayer == "w") ? "b" : "w";
    draw(ctx, canvas);
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
