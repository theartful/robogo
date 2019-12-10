let commands = {
    protocol_version: protocol_version,
    name: name,
    version: version,
    known_command: known_command,
    list_commands: list_commands,
    quit: quit,
    boardsize: boardsize,
    clear_board: clear_board,
    komi: komi,
    fixed_handicap: fixed_handicap,
    place_free_handicap: place_free_handicap,
    set_free_handicap: set_free_handicap,
    play: play,
    genmove: genmove,
    undo: undo,
    time_settings: time_settings,
    time_left: time_left,
    final_score: final_score,
    final_status_list: final_status_list,
    loadsgf: loadsgf,
    reg_genmove: reg_genmove,
    showboard: showboard,
    setboard: setboard,
}

/**
 * @param   {string}    command Command Name
 * @param   {Array}     args    Array of objects that contains command arguments
 * @returns {string}    request string containing required request
 */
let makeRequest = (command, args, id=null) => {
    if (!commandsList.includes(command))
        throw "Invalid Argument: command doesn't exist";
    
    if (id !== null && !isInt(id))
        throw "Invalid Argument: id must be an integer";
    
    let commandArgs = getArrowFunctionArgList(commands[command]);

    if (!arraysMatch(commandArgs, Object.keys(args)))
        throw `Invalid Argument: ${command} arguments list should be [${commandArgs}], however, [${Object.keys(args)}] was provided`;

    let request = "";
    if (id !== null)
        request += `${id} `;
    
    request += command;
    let argValues = Object.values(args);

    for (let index in argValues)
        request += ` ${argValues[index].toString()}`;

    request += "\n"
    return request;
}

/**
 * @param   {string}    command Command Name
 * @param   {Array}     args    Array of objects that contains command arguments
 * @returns {string}    request string containing required request
 */
let parseRequest = (request) => {
    if (typeof request !== "string")
        throw `request must be string, however ${typeof request} was passed`;

    request = request.replace(/\r?\n|\r/g, '');
    request = request.split(' ');
    let id = null;
    let command = null;
    let args = null;
    if (parseInt(request[0], 10).toString() === request[0]) {
        id = toInt(request[0]);
        command = request[1];
        args = request.slice(2);
    }
    else {
        command = request[0];
        args = request.slice(1);
    }
    
    return { id: id, command: command, args: args };
}

/**
 * @param   {string}    request     GTP command
 * @returns {string}    response    string containing GTP response in case of success or GTP error in case of failure
 */
let takeRequest = (request) => {
    let parsedRequest = null;
    try {
        parsedRequest = parseRequest(request);
    }
    catch(exception) {
        return `? ${exception}\n\n`;
    }
    
    let id = parsedRequest.id;
    let command = parsedRequest.command;
    let args = parsedRequest.args;
    let errorPrefix = (id !== null) ? `?${id}` : "?";
    let responsePrefix = (id !== null) ? `=${id}` : "=";
    if (!commandsList.includes(command))
        return `${errorPrefix} command doesn't exist\n\n`;
    
    if (command === "play")
        args = [args.join(' ')];

    let commandArgs = getArrowFunctionArgList(commands[command]);
    if (commandArgs.length !== args.length)
        return `${errorPrefix} ${command} arguments doesn't match\n\n`;
    
    try {
        let response = commands[command](...args);
        if (response === "break")
        {
            genmoveId = (id === null || id === undefined) ? '' : id.toString();
            return "break";
        }
        else if (response !== undefined)
            return `${responsePrefix} ${response.toString()}\n\n`;
        else
            return `${responsePrefix}\n\n`;
    }
    catch(exception) {
        return `? ${exception}\n\n`;
    }
}
