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
  ctx.clearRect(0, 0, c.height, c.width);
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

function updateBoard(moves_log, position) {
  for (var i = 0; i <board.length; i++){
    board[i].fill(0);
  }
  var move;
  for(var i = 0; i < moves_log.length; i++) {
    if (i > position)
      return;
    move = moves_log[i];
    board[move[0]][move[1]] = move[2];
  }
}

var moves_log = [[0,0,"w"], [8,16,"b"], [7,12,"w"], [7,17,"b"], [9,13,"w"], [18,18,"b"]];
var movestr;

for(i=0; i<moves_log.length; i++){  
  movestr = moves_log[i][2] + " (" + moves_log[i][0] + ", " + moves_log[i][1] + ")"
  $("#movesContainer ").append('<a href="#" id ='+i+' class="list-group-item list-group-item-action moveLink">'+ movestr +'</a>');
}

$(".list-group .list-group-item").click(function(e) {
  $(".list-group .list-group-item").removeClass("active");
  $(e.target).addClass("active");
});

var position = moves_log.length-1;
$(".moveLink").on("click", handleLinkClick);
function handleLinkClick() {
  position = parseInt($(this).attr("id"));
  updateBoard(moves_log, position);
  drawBoard();
  drawAllPieces();
}

drawBoard();
updateBoard(moves_log, position);
drawAllPieces();
