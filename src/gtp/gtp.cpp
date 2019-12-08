#include "gtp.h"
#include <algorithm>
#include <iterator>
#include <sstream>
#include <stdexcept>
using namespace gtp;
#include <functional>
#include <map>
using namespace std;

map<string, int> commands_args = {{"protocol_version", 0},
                                  {"name", 0},
                                  {"version", 0},
                                  {"known_command", 1},
                                  {"list_commands", 0},
                                  {"quit", 0},
                                  {"boardsize", 1},
                                  {"clear_board", 0},
                                  {"komi", 1},
                                  {"fixed_handicap", 1},
                                  {"place_free_handicap", 1},
                                  {"set_free_handicap", 1},
                                  {"play", 2},
                                  {"genmove", 1},
                                  {"undo", 0},
                                  {"time_settings", 3},
                                  {"time_left", 3},
                                  {"final_score", 0},
                                  {"final_status_list", 1},
                                  {"loadsgf", 2},
                                  {"reg_genmove", 1},
                                  {"showboard", 0}};

/**
 * @param   {string}    command Command Name
 * @param   {Array}     args    Array of objects that contains command arguments
 * @returns {string}    request string containing required request
 */
string gtp::make_request(string command, vector<string> args, int id)
{

	// Validate Command
	if (gtp::known_command(command).val() == "false")
	{
		return "Invalid Argument: command doesn't exist";
	}

	// Validate id
	if (id <= 0 && id != -1)
		throw std::invalid_argument(
		    "Invalid Argument: id must be a positive integer");

	// Validate arguments
	int required_args = commands_args[command];
	int given_args = args.size();
	if (given_args != required_args)
	{
		throw std::invalid_argument(
		    "Invalid Argument: " + command + " arguments list should be " +
		    to_string(required_args) + ", however, " + to_string(given_args) +
		    " was provided");
	}

	// Costruct request
	string request = "";
	if (id > 0)
		request += to_string(id) + " ";

	request += command;

	// concatenate arguments
	for (uint32_t i = 0; i < args.size(); i++)
		request += " " + args[i];

	request += "\n";
	return request;
}

std::pair<uint32_t, string> gtp::parse_response(string response)
{
	response.erase(
	    std::remove(response.begin(), response.end(), '\n'), response.end());
	if (response[0] == '?')
		throw std::invalid_argument(
		    "recieved error from client " + response.substr(1));

	response = response.substr(1);
	uint32_t id = 0;
	vector<string> result;
	std::istringstream iss(response);
	string id_str;
	iss >> id_str;

	bool has_id = !id_str.empty() &&
	              std::find_if(id_str.begin(), id_str.end(), [](char c) {
		              return !std::isdigit(c);
	              }) == id_str.end();
	if (!has_id)
		return std::pair<uint32_t, string>(0, response);

	std::stringstream id_stream;
	id_stream << id_str;
	id_stream >> id;
	string res_args;
	iss >> res_args;
	for (string req; iss >> req;)
		res_args += " " + req;

	return std::pair<uint32_t, string>(id, res_args);
}

/**
 * @param   {string}    request     GTP command
 * @returns {string}    response    string containing GTP response in case of
 * success or GTP error in case of failure
 */
string gtp::take_request(string request)
{
	vector<string> result;
	istringstream iss(request);
	int id = 0;
	string command;
	vector<string> args;
	try
	{
		for (string temp_request; iss >> temp_request;)
		{
			result.push_back(temp_request);
		}

		// assign command and arg
		try
		{
			id = stoi(result[0]);
			command = result[1];
			args = vector<string>(result.begin() + 2, result.end());

			// Validate ID
			if (id <= 0)
			{
				return "?" + to_string(id) +
				       " Invalid Argument: id must be a positive integer\n\n";
			}
		}
		catch (std::invalid_argument& e)
		{
			command = result[0];
			args = vector<string>(result.begin() + 1, result.end());
		}
	}
	catch (exception& ex)
	{
		return "? " + string(ex.what()) + "\n\n";
	}

	string id_ = (id) ? to_string(id) + " " : " ";

	// Validate Command
	if (known_command(command).val() == "false")
	{
		return "?" + id_ + "command doesn't exist\n\n";
	}

	// Validate arguments
	int required_args = commands_args[command];
	int given_args = args.size();
	if (given_args != required_args)
	{
		return "?" + id_ + "Invalid Argument: " + command +
		       " arguments list should be " + to_string(required_args) +
		       ", however, " + to_string(given_args) + " was provided\n\n";
	}
	// call corresponding functions

	string response = "=" + id_;

	if (command == "protocol_version")
	{
		return response + to_string(protocol_version()) + "\n\n";
	}
	else if (command == "name")
	{
		return response + name().val() + "\n\n";
	}
	else if (command == "version")
	{
		return response + version().val() + "\n\n";
	}
	else if (command == "known_command")
	{
		return response + known_command(args[0]).val() + "\n\n";
	}
	else if (command == "quit")
	{
		quit();
		return response + "Quit" + "\n\n";
	}
	else if (command == "clear_board")
	{
		clear_board();
		return response + "Cleared Board" + "\n\n";
	}
	else if (command == "komi")
	{
		try
		{
			komi(stof(args[0]));
			return response + "Komi" + "\n\n";
		}
		catch (std::invalid_argument& e)
		{
			return "?" + id_ + "Invalid arguments\n\n";
		}
	}
	else if (command == "play")
	{
		Move move(args[0], args[1]);
		play(move);
		return response + "Moved " + args[0] + " to " + args[1] + "\n\n";
	}
	else if (command == "genmove")
	{
		// return response+genmove(args[0]).val()+"\n\n";
	}
	else if (command == "undo")
	{
		undo();
		return response + "Undo" + "\n\n";
	}
	else if (command == "final_score")
	{
		return response + final_score() + "\n\n";
	}
	else if (command == "loadsgf")
	{
		try
		{
			loadsgf(args[0], uint32_t(stoi(args[1])));
			return response + "LoadSGF" + "\n\n";
		}
		catch (std::invalid_argument& e)
		{
			return "?" + id_ + "Invalid arguments\n\n";
		}
	}
	else if (command == "reg_genmove")
	{
		// Same problem as genmove
	}
	else if (command == "showboard")
	{
		return response + showboard().val() + "\n\n";
	}
	else
		return "?" + id_ + "command doesn't exist\n\n";

	return "";
}
// int main()
// {
// 	// vector<string> args = {"w","a15"};
// 	// vector<string> args ;
// 	// string request = makeRequest("protocol_version",args);
// 	string request = "showboard";
// 	string result = takeRequest(request);
// 	// cout<<request<<endl;
// 	cout << result << endl;
// 	return 0;
// }
