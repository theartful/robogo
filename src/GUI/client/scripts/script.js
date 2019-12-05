var c = document.getElementById("go-canvas");
var ctx = c.getContext("2d");
var black = document.getElementById("black");
var white = document.getElementById("white");

const startOffset = 20;
const arraySize = 18;
const endOffset = c.width - startOffset;
const step = (c.width - 2 * startOffset) / arraySize;

var board = new Array(arraySize + 1)
    .fill(0)
    .map(() => new Array(arraySize + 1).fill(0));

const drawBoard = () => {
    for (let xAxis = startOffset; xAxis <= endOffset; xAxis += step) {
        ctx.moveTo(xAxis, startOffset);
        ctx.lineTo(xAxis, endOffset);
        ctx.stroke();
    }

    for (let yAxis = startOffset; yAxis <= endOffset; yAxis += step) {
        ctx.moveTo(startOffset, yAxis);
        ctx.lineTo(endOffset, yAxis);
        ctx.stroke();
    }
};

const drawAllPieces = () => {
    let row = 0;
    for (let xAxis = startOffset - step / 2; xAxis < endOffset; xAxis += step) {
        let col = 0;
        for (let yAxis = startOffset - step / 2; yAxis < endOffset; yAxis += step) {
            if (board[row][col]) {
                if (board[row][col] == "w") {
                    ctx.drawImage(white, xAxis, yAxis, step, step);
                } else {
                    ctx.drawImage(black, xAxis, yAxis, step, step);
                }
            }
            col++;
        }
        row++;
    }
};

board[0][0] = "w";
board[7][12] = "w";
board[9][13] = "w";
board[8][16] = "b";
board[7][17] = "b";
board[5][16] = "b";
board[18][18] = "b";

drawBoard();
drawAllPieces();
