socket.onopen = function (e) {
    
};

socket.onmessage = function (event) {
    // alert(`[message] Data received from server: ${event.data}`);
    let command = event.data.split(' ');
    if (command.length === 3 && command[0] === "end") {
        let winnerName = (command[1] === "b") ? player1_name : player2_name;
        window.location = `./finish.html?winner=${winnerName}&score=${Number(command[2])}`;
    }
    else if (command.length === 5 && command[0] === "start") {
        player1 = command[1];
        player2 = command[2];
        player1_name = command[3];
        player2_name = command[4];
        updateNames(player1_name, player2_name);
        stopWaiting();
    }
    else {
        let response = takeRequest(event.data);
        if (response !== "break")
            socket.send(response);
    }
};

socket.onclose = function (event) {
    // if (event.wasClean) {
    //     alert(`[close] Connection closed cleanly, code=${event.code} reason=${event.reason}`);
    // } else {
    //     // e.g. server process killed or network down
    //     // event.code is usually 1006 in this case
    //     alert('[close] Connection died');
    // }
};

socket.onerror = function (error) {
    alert(`[error] ${error.message}`);
};
