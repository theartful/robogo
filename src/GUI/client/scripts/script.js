var canvas = document.getElementById("go-canvas");
var ctx = canvas.getContext("2d");
var black = document.getElementById("black");
var white = document.getElementById("white");

function getUrlVars() {
    var vars = {};
    var parts = window.location.href.replace(/[?&]+([^=&]+)=([^&]*)/gi, function (
        m,
        key,
        value
    ) {
        vars[key] = value;
    });
    return vars;
}

const updateNames = (player1_name, player2_name) => {
    $("#player1_name").text(player1_name);
    $("#player2_name").text(player2_name);
};

var urlParams = getUrlVars();
player1 = urlParams["player1"] ? urlParams["player1"] : "cmp";
player2 = urlParams["player2"] ? urlParams["player2"] : "cmp";
player1_name = urlParams["player1_name"] ? urlParams["player1_name"] : "CMP1";
player2_name = urlParams["player2_name"] ? urlParams["player2_name"] : "CMP2";

let allowMove = false;
let pieceLocation = { location: [] };

currentTurn = { name: player1_name, mode: player1 };

updateNames(player1_name, player2_name);

var clock1 = $(".clock").FlipClock(15 * 60, {
    clockFace: "MinuteCounter",
    countdown: true,
    callbacks: {
        stop: function () {
            // $(".message").html("The clock has stopped!");
        }
    }
});
var clock2 = $(".clock2").FlipClock(15 * 60, {
    clockFace: "MinuteCounter",
    countdown: true,
    autoStart: false,
    callbacks: {
        stop: function () {
            // $(".message").html("The clock has stopped!");
        }
    }
});

const swalWithBootstrapButtons = Swal.mixin({
    customClass: {
        confirmButton: "btn btn-success",
        cancelButton: "btn btn-danger"
    },
    buttonsStyling: false
});

$(".player2-controls.resign").on("click", function (event) {
    if ($(this).hasClass("disabled")) {
    } else {
        score = 100;
        swalWithBootstrapButtons
            .fire({
                title: "Are you sure?",
                text: "You will lose the game!",
                icon: "warning",
                showCancelButton: true,
                confirmButtonText: "No, cancel!",
                cancelButtonText: "Yes",
                reverseButtons: true
            })
            .then(result => {
                if (result.value) {
                    swalWithBootstrapButtons.fire(
                        "Cancelled",
                        "Your game is safe :)",
                        "success"
                    );
                } else if (
                    /* Read more about handling dismissals below */
                    result.dismiss === Swal.DismissReason.cancel
                ) {
                    // ---------------------------------- to do --------------------------------
                    window.location = `./finish.html?winner=${player1_name}&score=${score}`;
                }
            });
    }
});

$(".player1-controls.resign").on("click", function (event) {
    if ($(this).hasClass("disabled")) {
    } else {
        score = 100;
        swalWithBootstrapButtons
            .fire({
                title: "Are you sure?",
                text: "You will lose the game!",
                icon: "warning",
                showCancelButton: true,
                confirmButtonText: "No, cancel!",
                cancelButtonText: "Yes",
                reverseButtons: true
            })
            .then(result => {
                if (result.value) {
                    swalWithBootstrapButtons.fire(
                        "Cancelled",
                        "Your game is safe :)",
                        "success"
                    );
                } else if (
                    /* Read more about handling dismissals below */
                    result.dismiss === Swal.DismissReason.cancel
                ) {
                    // ---------------------------------- to do --------------------------------
                    window.location = `./finish.html?winner=${player2_name}&score=${score}`;
                }
            });
    }
});

$(".player1-controls.pass").on("click", function (event) {
    if (player1 === "human") {
        pieceLocation.location = null;
    }
});
$(".player2-controls.pass").on("click", function (event) {
    if (player2 === "human") {
        pieceLocation.location = null;
    }
});


const changeTurn = (playerTime) => {
    currentTurn =
        currentTurn.name == player1_name
            ? { name: player2_name, mode: player2 }
            : { name: player1_name, mode: player1 };
    if (currentTurn.name == player1_name) {
        $("#player1_name").addClass("flux-animation");
        $("#player2_name").removeClass("neon-animation");
        $(".player1-controls").removeClass("disabled");
        $(".player2-controls").addClass("disabled");
        currentPlayer = changePlayer(currentPlayer);
        clock1.setTime(playerTime);
        clock1.start();
        clock2.stop();
    } else {
        $("#player2_name").addClass("neon-animation");
        $("#player1_name").removeClass("flux-animation");
        $(".player2-controls").removeClass("disabled");
        $(".player1-controls").addClass("disabled");
        currentPlayer = changePlayer(currentPlayer);
        clock2.setTime(playerTime);
        clock2.start();
        clock1.stop();
    }
};

const updatePlayer1Score = score => {
    $("#player1_score").text(score);
};
const updatePlayer2Score = score => {
    $("#player2_score").text(score);
};
const updatePlayer1Stones = stones => {
    $("#player1_stones").text(stones);
};
const updatePlayer2Stones = stones => {
    $("#player2_stones").text(stones);
};

const startOffset = 20;
const arraySize = 18;
const endOffset = canvas.width - startOffset;
const step = (canvas.width - 2 * startOffset) / arraySize;

let currentPlayer = "b";

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
        ctx.strokeStyle = "black";
        ctx.lineWidth = 1;
        ctx.stroke();
    }
    ctx.lineWidth = 0;
};

const drawAllPieces = () => {
    let row = 0;
    for (let xAxis = startOffset - step / 2; xAxis < endOffset; xAxis += step) {
        let col = 0;
        for (let yAxis = startOffset - step / 2; yAxis < endOffset; yAxis += step) {
            if (board[row][col]) {
                if (board[row][col].color == "w") {
                    ctx.drawImage(white, xAxis, yAxis, step, step);
                } else if (board[row][col].color == "b") {
                    ctx.drawImage(black, xAxis, yAxis, step, step);
                }
            }
            col++;
        }
        row++;
    }
};

var addSound = new Audio("./audio/add-sound.mp3");
var removeSound = new Audio("./audio/remove-sound.mp3");
const removePiece = (xIndex, yIndex) => {
    board[xIndex][yIndex] = null;
    removeSound.play();
    draw(ctx, canvas);
};

// board[5][5] = { color: "w", selected: false };

$("canvas#go-canvas").on("mousemove", function (event) {
    if (allowMove) {
        draw(ctx, canvas);
        var xIndex = parseInt((event.offsetX - (startOffset - step / 2)) / step);
        var yIndex = parseInt((event.offsetY - (startOffset - step / 2)) / step);
        xAxis = xIndex * step + startOffset;
        yAxis = yIndex * step + startOffset;
        if (currentPlayer == "w") {
            ctx.strokeStyle = "rgba(255,255,255,1)";
            ctx.fillStyle = "rgba(255,255,255,0.5)";
        } else {
            ctx.strokeStyle = "rgba(2,2,2,1)";
            ctx.fillStyle = "rgba(2,2,2,0.5)";
        }
        ctx.lineWidth = -1;
        ctx.beginPath();
        ctx.arc(xAxis, yAxis, step / 2, 0, 2 * Math.PI);
        ctx.fill();
        ctx.stroke();
    }
});

const addPiece = (xIndex, yIndex) => {
    board[xIndex][yIndex] = { color: currentPlayer };
    changeTurn();
    addSound.play();
    draw(ctx, canvas);
};

$("canvas#go-canvas").on("click", function (event) {
    if (allowMove) {
        var xIndex = parseInt((event.offsetX - (startOffset - step / 2)) / step);
        var yIndex = parseInt((event.offsetY - (startOffset - step / 2)) / step);
    
        if (xIndex < 0 || xIndex > arraySize || yIndex < 0 || yIndex > arraySize) {
            return;
        }
    
        pieceLocation.location = [xIndex, yIndex];
    }
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
