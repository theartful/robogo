#include "gtp.h"
using namespace gtp;

const string gtp::commands[] = {"protocol_version",
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
                           "showboard"};
List<string> gtp::commands_list(gtp::commands, 22u);

// List<string> commandsList= vector<string>{
//   "protocol_version",
//   "name",
//   "version",
//   "known_command",
//   "list_commands",
//   "quit",
//   "boardsize",
//   "clear_board",
//   "komi",
//   "fixed_handicap",
//   "place_free_handicap",
//   "set_free_handicap",
//   "play",
//   "genmove",
//   "undo",
//   "time_settings",
//   "time_left",
//   "final_score",
//   "final_status_list",
//   "loadsgf",
//   "reg_genmove",
//   "showboard"
//  };
// Adminstrative Commands
/**
 * @param   none
 * @returns {int}   version_number    Protocol Version Number
 */
uint32_t gtp::protocol_version()
{
	return 2u;
}

/**
 * @param   none
 * @returns {List<string>}   name    Engine Name
 */
List<string> gtp::name()
{
  List<string> name({"Go","Slayer"});
  return name;
}

/**
 * @param   none
 * @returns {List<string>}   version    Engine Version Name
 */
List<string> gtp::version()
{
  List<string> version({"1.0.0"});
  return version;
}

/**
 * @param   {string}    command_name    Name of the command to check that it
 * exist
 * @returns {Boolean}   known           "true" if command is known, "false"
 * otherwise
 */

Boolean gtp::known_command(string command_name)
{
	return Boolean(gtp::commands_list.includes(command_name));
}

/**
 * @param   none
 * @returns {MultiLineList<string>}   commands  List of commands, one per row
 */
MultiLineList<string> gtp::list_commands()
{
  // MultiLineList<string> list ;
  // list.appendAll(commandsList.items);
  // return list;
	MultiLineList<string> list(gtp::commands, 22u);
	return list;
}

/**
 * @param   none
 * @returns {void}
 */

void gtp::quit()
{
}
