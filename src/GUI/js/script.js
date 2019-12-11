var canvas = document.getElementById("go-canvas");
var ctx = canvas.getContext("2d");
var black = document.getElementById("black");
var white = document.getElementById("white");

const startOffset = 20;
const arraySize = 18;
const endOffset = canvas.width - startOffset;
const step = (canvas.width - 2 * startOffset) / arraySize;

let currentPlayer = "b";
let selectedPiece = null;

var board = new Array(arraySize + 1)
  .fill(null)
  .map(() => new Array(arraySize + 1).fill(null));

const drawBoard = () => {
  for (let xAxis = startOffset; xAxis <= endOffset; xAxis += step) {
    ctx.moveTo(xAxis, startOffset);
    ctx.lineTo(xAxis, endOffset);
    ctx.strokeStyle = "black";
    ctx.lineWidth = 1;
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
        if (board[row][col].color == "w") {
          if (board[row][col].selected) {
            drawSelect(ctx, xAxis, yAxis);
          }
          ctx.drawImage(white, xAxis, yAxis, step, step);
        } else {
          if (board[row][col].selected) {
            drawSelect(ctx, xAxis, yAxis);
          }
          ctx.drawImage(black, xAxis, yAxis, step, step);
        }
      }
      col++;
    }
    row++;
  }
};

// board[0][0] = { color: "w", selected: true };

$("canvas#go-canvas").on("mousemove", function(event) {
  var xIndex = parseInt((event.offsetX - (startOffset - step / 2)) / step);
  var yIndex = parseInt((event.offsetY - (startOffset - step / 2)) / step);
  xAxis = xIndex * step + startOffset;
  yAxis = yIndex * step + startOffset;
  draw(ctx, canvas);
  ctx.beginPath();
  ctx.arc(xAxis, yAxis, step / 2, 0, 2 * Math.PI);
  ctx.fillStyle = "rgba(2,2,2,0.2)";
  ctx.lineWidth = 0;
  ctx.fill();
  // ctx.strokeStyle = "rgba(2,2,2,0.2)";
  // ctx.stroke();
});

const clearSelected = () => {
  board.map((value, index) => {
    value.map((val, idx) => {
      return val ? (val.selected = false) : null;
    });
  });
};

const deselectPiece = (xIndex, yIndex) => {
  board[xIndex][yIndex].selected = false;
  selectedPiece = null;
  draw(ctx, canvas);
  return;
};

const selectPiece = (xIndex, yIndex) => {
  board[selectedPiece.xIndex][selectedPiece.yIndex] = null;
  board[xIndex][yIndex] = {
    color: selectedPiece.color,
    selected: false
  };
  selectedPiece = null;
  currentPlayer = changePlayer(currentPlayer);
  draw(ctx, canvas);
  return;
};

const movePiece = (xIndex, yIndex) => {
  clearSelected();
  board[xIndex][yIndex] = {
    color: currentPlayer,
    selected: true
  };
  selectedPiece = { xIndex: xIndex, yIndex: yIndex, color: currentPlayer };
  draw(ctx, canvas);
  return;
};

const addPiece = (xIndex, yIndex) => {
  board[xIndex][yIndex] = { color: currentPlayer, selected: false };
  currentPlayer = changePlayer(currentPlayer);
  draw(ctx, canvas);
};

$("canvas#go-canvas").on("click", function(event) {
  var xIndex = parseInt((event.offsetX - (startOffset - step / 2)) / step);
  var yIndex = parseInt((event.offsetY - (startOffset - step / 2)) / step);

  if (xIndex < 0 || xIndex > arraySize || yIndex < 0 || yIndex > arraySize) {
    return;
  }

  if (selectedPiece) {
    if (selectedPiece.xIndex == xIndex && selectedPiece.yIndex == yIndex) {
      return deselectPiece(xIndex, yIndex);
    }
    return selectPiece(xIndex, yIndex);
  }

  if (board[xIndex][yIndex]) {
    if (board[xIndex][yIndex].color == currentPlayer) {
      return movePiece(xIndex, yIndex);
    } else {
      $.growl({
        title: "Error",
        message: "This is not your piece",
        style: "error"
      });
      return;
    }
  }
  return addPiece(xIndex, yIndex);
});

const changePlayer = currentPlayer => {
  if (currentPlayer == "w") {
    $("span.player").text("Black");
    return "b";
  } else {
    $("span.player").text("White");
    return "w";
  }
};

const draw = (ctx, canvas) => {
  ctx.clearRect(0, 0, canvas.width, canvas.height);
  drawBoard();
  drawAllPieces();
};

draw(ctx, canvas);

/**
 * clock
 */

$(document).ready(function() {
  clock = $(".clock").FlipClock(15*60, {
    clockFace: "MinuteCounter",
    countdown: true,
    callbacks: {
      stop: function() {
        // $(".message").html("The clock has stopped!");
      }
    }
  });
  clock = $(".clock2").FlipClock(15*60, {
    clockFace: "MinuteCounter",
    countdown: true,
    callbacks: {
      stop: function() {
        // $(".message").html("The clock has stopped!");
      }
    }
  });
});