#include "gtp.h"
#include <algorithm>
#include <functional>
#include <iterator>
#include <sstream>
#include <stdexcept>
using namespace gtp;

const std::string GTPEngine::commands[] = {"protocol_version",
                                           "name",
                                           "version",
                                           "known_command",
                                           "list_commands",
                                           "quit",
                                           "boardsize",
                                           "clear_board",
                                           "komi",
                                           "play",
                                           "genmove",
                                           "undo",
                                           "final_score",
                                           "final_status_list",
                                           "showboard",
                                           "setboard"};

const std::map<string, uint32_t> GTPEngine::commands_args = {
    {"protocol_version", 0U},
    {"name", 0U},
    {"version", 0U},
    {"known_command", 1U},
    {"list_commands", 0U},
    {"quit", 0U},
    {"boardsize", 1U},
    {"clear_board", 0U},
    {"komi", 1U},
    {"play", 1U},
    {"genmove", 1U},
    {"undo", 0U},
    {"final_score", 0U},
    {"final_status_list", 1U},
    {"showboard", 0U},
    {"setboard", 1U}};

/**
 * @param   {string}    command Command Name
 * @param   {Array}     args    Array of objects that contains command arguments
 * @returns {string}    request string containing required request
 */
std::string GTPEngine::make_request(
    std::string command, std::vector<std::string> args, uint32_t id)
{
	// Validate Command
	if (known_command(command).val() == "false")
		throw std::invalid_argument("Invalid Argument: command doesn't exist");

	// Validate arguments
	if (commands_args.at(command) != args.size())
	{
		throw std::invalid_argument(
		    "Invalid Argument: " + command + " arguments list should be " +
		    to_string(commands_args.at(command)) + ", however, " +
		    to_string(args.size()) + " was provided");
	}

	// Costruct request
	std::string request = "";
	if (id > 0)
		request += to_string(id) + " ";

	request += command;

	// concatenate arguments
	for (uint32_t i = 0; i < args.size(); i++)
		request += " " + args[i];

	request += "\n";
	return request;
}

std::pair<uint32_t, std::vector<std::string>>
GTPEngine::tokenize(std::string str)
{
	std::pair<uint32_t, std::vector<std::string>> result;
	std::istringstream stream(str);
	std::string id_str;
	stream >> id_str;

	bool has_id = !id_str.empty() &&
	              std::find_if(id_str.begin(), id_str.end(), [](char c) {
		              return !std::isdigit(c);
	              }) == id_str.end();

	
	if (!has_id)
	{
		stream = std::istringstream(str);
		result.first = 0;
	}
	else
	{
		uint32_t id = 0;
		std::stringstream id_stream(id_str);
		id_stream >> id;
		result.first = id;
	}

	std::vector<std::string> tokens;
	for (std::string token; stream >> token;)
		tokens.push_back(token);

	result.second = tokens;
	return result;
}

std::pair<uint32_t, std::string> GTPEngine::parse_response(std::string response)
{
	response.erase(
	    std::remove(response.begin(), response.end(), '\n'), response.end());
	if (response[0] == '?')
		throw std::invalid_argument(
		    "recieved error from client " + response.substr(1));

	response = response.substr(1);
	std::pair<uint32_t, std::vector<std::string>> tokens = tokenize(response);
	std::vector<std::string> args = tokens.second;
	if (args.size() <= 0)
		return std::pair<uint32_t, string>(tokens.first, "");

	std::string args_str = args[0];
	for (uint32_t i = 1; i < args.size(); i++)
		args_str = " " + args[i];

	return std::pair<uint32_t, std::string>(tokens.first, args_str);
}

std::string GTPEngine::parse_request(
    std::string request, std::string& error_prefix,
    std::string& response_prefix, std::string& command,
    std::vector<std::string>& args)
{
	std::pair<uint32_t, std::vector<std::string>> parsed_request =
	    tokenize(request);
	response_prefix = (parsed_request.first > 0)
	                      ? "=" + std::to_string(parsed_request.first) + " "
	                      : "= ";
	error_prefix = (parsed_request.first > 0)
	                   ? "?" + std::to_string(parsed_request.first) + " "
	                   : "? ";

	std::vector<std::string> tokens = parsed_request.second;
	if (tokens.size() <= 0)
		return error_prefix + "invalid command doesn't exsist\n\n";

	command = tokens[0];
	args;
	if (command == "play")
	{
		std::string move = "";
		for (uint32_t i = 1; i < tokens.size(); i++)
		{
			if (i == 1)
				move = tokens[i];
			else
				move += " " + tokens[i];
		}
		args.push_back(move);
	}
	else
	{
		for (uint32_t i = 1; i < tokens.size(); i++)
			args.push_back(tokens[i]);
	}

	// Validate Command
	if (known_command(command).val() == "false")
		return error_prefix + "command doesn't exist\n\n";

	// Validate arguments
	if (commands_args.at(command) != args.size())
		return error_prefix + "Invalid Argument: " + command +
		       " arguments list should be " +
		       to_string(commands_args.at(command)) + ", however, " +
		       to_string(args.size()) + " was provided\n\n";

	return "";
}

/**
 * @param   {string}    request     GTP command
 * @returns {string}    response    string containing GTP response in case of
 * success or GTP error in case of failure
 */
std::string GTPEngine::take_request(std::string request)
{
	std::string error_prefix = "";
	std::string response_prefix = "";
	std::string command = "";
	std::vector<std::string> args;
	std::string error =
	    parse_request(request, error_prefix, response_prefix, command, args);
	if (error != "")
		return error;

	try
	{
		if (command == "protocol_version")
			return response_prefix + to_string(protocol_version()) + "\n\n";
		else if (command == "name")
			return response_prefix + name().val() + "\n\n";
		else if (command == "version")
			return response_prefix + version().val() + "\n\n";
		else if (command == "known_command")
			return response_prefix + known_command(args[0]).val() + "\n\n";
		else if (command == "list_commands")
			return response_prefix + list_commands().val() + "\n\n";
		else if (command == "quit")
		{
			quit();
			return response_prefix + "\n\n";
		}
		else if (command == "clear_board")
		{
			clear_board();
			return response_prefix + "\n\n";
		}
		else if (command == "komi")
			return response_prefix + "\n\n";
		else if (command == "boardsize")
			return response_prefix + "\n\n";
		else if (command == "play")
		{
			play(Move(args[0]));
			return response_prefix + "\n\n";
		}
		else if (command == "genmove")
			return response_prefix + genmove(Color(args[0])) + "\n\n";
		else if (command == "final_score")
			return response_prefix + final_score() + "\n\n";
		else if (command == "showboard")
			return response_prefix + showboard() + "\n\n";
		else
			return error_prefix + "command doesn't exist\n\n";
	}
	catch (const std::exception& e)
	{
		return error_prefix + e.what() + "\n\n";
	}
}
