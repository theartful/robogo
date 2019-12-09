#include "gtp.h"
using namespace gtp;

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
	List<string> name({"Go", "Slayer"});
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
    bool known = std::find(std::begin(gtp::commands), std::end(gtp::commands), command_name) != std::end(gtp::commands);
	return Boolean(known);
}

/**
 * @param   none
 * @returns {MultiLineList<string>}   commands  List of commands, one per row
 */
MultiLineList<string> gtp::list_commands()
{
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
