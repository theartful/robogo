const commandsList = [
    "protocol_version",
    "name",
    "version",
    "known_command",
    "list_commands",
    "quit",
    "boardsize",
    "clear_board",
    "komi",
    "fixed_handicap",
    "place_free_handicap",
    "set_free_handicap",
    "play",
    "genmove",
    "undo",
    "time_settings",
    "time_left",
    "final_score",
    "final_status_list",
    "loadsgf",
    "reg_genmove",
    "showboard",
    "setboard"
];

// Adminstrative Commands
/**
 * @param   none
 * @returns {int}   version_number    Protocol Version Number
 */
let protocol_version = () => {
    return 2;
}

/**
 * @param   none
 * @returns {List<string>}   name    Engine Name
 */
let name = () => {
    let name = entities.List("string");
    name.append("Go");
    name.append("Slayer");

    return name;
}

/**
 * @param   none
 * @returns {List<string>}   version    Engine Version Name
 */
let version = () => {
    let version = List("string");
    version.append("1.0.0");

    return version;
}

/**
 * @param   {string}    command_name    Name of the command to check that it exist
 * @returns {Boolean}   known           "true" if command is known, "false" otherwise
 */
let known_command = (command_name) => {
    return commandsList.includes(command_name);
}

/**
 * @param   none
 * @returns {MultiLineList<string>}   commands  List of commands, one per row
 */
let list_commands = () => {
    let commands = MultiLineList("string");
    commands.appendAll(commandsList);

    return commands;
}

/**
 * @param   none
 * @returns {void}
 */
let quit = () => {
    // do nothing
}
