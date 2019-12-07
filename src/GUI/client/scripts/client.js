// let socket = new WebSocket("ws://localhost:9002");

socket.onopen = function (e) {
    alert("[open] Connection established");
};

socket.onmessage = function (event) {
    // alert(`[message] Data received from server: ${event.data}`);
    let response = takeRequest(event.data);
    if (response !== "break")
        socket.send(response);
};

socket.onclose = function (event) {
    if (event.wasClean) {
        alert(`[close] Connection closed cleanly, code=${event.code} reason=${event.reason}`);
    } else {
        // e.g. server process killed or network down
        // event.code is usually 1006 in this case
        alert('[close] Connection died');
    }
};

socket.onerror = function (error) {
    alert(`[error] ${error.message}`);
};

// sendMessage = function() {
//     let message = document.getElementById("message").value;
//     alert("sending message to server!!!");
//     socket.send(message);
// }
