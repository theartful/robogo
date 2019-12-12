// let socket = new WebSocket("ws://localhost:9002");

socket.onopen = function (e) {
    // alert("[open] Connection established");
    let mode1 = "a";
    if (player1 === "human")
        mode1 = "h";
    else if (player1 === "remote")
        mode1 = "r";
    
    let mode2 = "a";
    if (player2 === "human")
        mode2 = "h";
    else if (player2 === "remote")
        mode2 = "r";

    socket.send(`game_config ${mode1} ${mode2}`);
};

socket.onmessage = function (event) {
    // alert(`[message] Data received from server: ${event.data}`);
    let command = event.data.split(' ');
    if (command.length === 3 && command[0] === "end") {
        let winnerName = (command[1] === "b") ? player1_name : player2_name;
        window.location = `./finish.html?winner=${winnerName}&score=${command[2]}`;
    }
    else {
        let response = takeRequest(event.data);
        if (response !== "break")
            socket.send(response);
    }
};

socket.onclose = function (event) {
    if (event.wasClean) {
        // alert(`[close] Connection closed cleanly, code=${event.code} reason=${event.reason}`);
    } else {
        // e.g. server process killed or network down
        // event.code is usually 1006 in this case
        // alert('[close] Connection died');
    }
};

socket.onerror = function (error) {
    alert(`[error] ${error.message}`);
};
