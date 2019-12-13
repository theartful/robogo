#include "gtp.h"
using namespace gtp;

// Adminstrative Commands
/**
 * @param   none
 * @returns {int}   version_number    Protocol Version Number
 */
uint32_t GTPEngine::protocol_version()
{
	return 2u;
}

/**
 * @param   none
 * @returns {List<string>}   name    Engine Name
 */
List<string> GTPEngine::name()
{
	List<string> name({"Go", "Slayer"});
	return name;
}

/**
 * @param   none
 * @returns {List<string>}   version    Engine Version Name
 */
List<string> GTPEngine::version()
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
Boolean GTPEngine::known_command(string command_name)
{
	bool known = false;
	for (uint32_t i = 0; i < 16; i++)
	{
		if (GTPEngine::commands[i] == command_name)
		{
			known = true;
			break;
		}
	}
	return Boolean(known);
}

/**
 * @param   none
 * @returns {MultiLineList<string>}   commands  List of commands, one per row
 */
MultiLineList<string> GTPEngine::list_commands()
{
	MultiLineList<string> list(GTPEngine::commands, NUM_COMMANDS);
	return list;
}

/**
 * @param   none
 * @returns {void}
 */
void GTPEngine::quit()
{
	exit(0);
}
