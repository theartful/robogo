#include "gtp.h"
#include "entities.h"

List<string> commandsList= {
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
  "showboard"
 };
// Adminstrative Commands


/**
 * @param   none
 * @returns {int}   version_number    Protocol Version Number
 */

uint32_t protocol_version()
{
  return 2;
}


/**
 * @param   none
 * @returns {List<string>}   name    Engine Name
 */

List<string> name()
{
  List<string> name;
  name.append("Go");
  name.append("Slayer");
  return name;
}


/**
 * @param   none
 * @returns {List<string>}   version    Engine Version Name
 */

List<string> version()
{
  List<string> version;
  version.append("1.0.0");
  return version;
}


/**
 * @param   {string}    command_name    Name of the command to check that it exist
 * @returns {Boolean}   known           "true" if command is known, "false" otherwise
 */

Boolean known_command(string command_name)
{
  return commandsList.includes(command_name);
}

/**
 * @param   none
 * @returns {MultiLineList<string>}   commands  List of commands, one per row
 */

MultiLineList<string> list_commands()
{
  MultiLineList<string> list ;
  list.appendAll(commandsList);
  return list;
}

/**
 * @param   none
 * @returns {void}
 */

void quit()
{

}
