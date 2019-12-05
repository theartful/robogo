// Tournament Commands
/**
 * @param   {int}   main_time       Main time measured in seconds
 * @param   {int}   byo_yomi_time   Byo yomi time measured in seconds
 * @param   {int}   byo_yomi_stones Number of stones per byo yomi period
 * @returns {void}
 */
let time_settings = (main_time, byo_yomi_time, byo_yomi_stones) => {
    try {
        main_time = toInt(main_time);
        byo_yomi_time = toInt(byo_yomi_time);
        byo_yomi_stones = toInt(byo_yomi_stones);
    }
    catch(exception) {
        throw "Invalid parameters must be integer values";
    }

    // does something
}

/**
 * @param   {Color} color   Color for which the information applies
 * @param   {int}   time    Number of seconds remaining
 * @param   {int}   stones  Number of stones remaining
 * @returns {void}
 */
let time_left = (color, time, stones) => {
    try {
        time = toInt(time);
        stones = toInt(stones);
    }
    catch(exception) {
        throw "Invalid parameters must be integer values";
    }
    color = new Color(color.toString());

    // does something
}

/**
 * @param   none
 * @returns {string}    score   final game score
 */
let final_score = () => {
    let score = "";

    // get game score

    return score;
}

/**
 * @param   {string}                        status  Requested status
 * @returns {MultiLineList<List<Vertex>>}   stones  Stones with the requested status
 */
let final_status_list = (status) => {
    if (typeof status !== "string")
        throw "invalid status value";

    let stones = MultiLineList("List");

    return stones;
}
