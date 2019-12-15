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
var urlParams = getUrlVars();
let port = (urlParams["port"]) ? Number(urlParams["port"]) : 9002;
let player1 = "";
let player2 = "";
let player1_name = "";
let player2_name = "";

let openSocket = () => {
    let uri = `ws://localhost:${port}`;
    console.log(`connection uri: ${uri}`);
    return new WebSocket(uri);
}

let socket = openSocket();
let closeConnection = () => {
    let url = window.location.href;
    if (url.length == 11 && url.includes("index.html")) {
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
    }

    socket.close();
}
