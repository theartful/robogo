#include "gtp.h"
#include <sstream>
#include <iterator>
#include <algorithm>
#include <stdexcept>
using namespace gtp;


/**
 * @param   {string}    command Command Name
 * @param   {Array}     args    Array of objects that contains command arguments
 * @returns {string}    request string containing required request
 */
string gtp::make_request(string command, vector<string> args, int id = -1)
{
	// Validate Command
	std::map<std::string, int>::iterator it;
	it = commandsList.find(command);
	if (it == commandsList.end())
		throw std::invalid_argument("Invalid Argument: command doesn't exist");

	// Validate id
	if (id <= 0 && id != -1)
		throw std::invalid_argument("Invalid Argument: id must be a positive integer");

	// Validate arguments
	int required_args = commandsList[command];
	int given_args = args.size();
	if (given_args != required_args)
	{
		throw std::invalid_argument("Invalid Argument: " + command + " arguments list should be " +
		       to_string(required_args) + ", however, " +
		       to_string(given_args) + " was provided");
	}

	// Costruct request
	string request = "";
	if (id > 0)
		request += to_string(id) + " ";

	request += command;

	// concatenate arguments
	for (int i = 0; i < args.size(); i++)
		request += " " + args[i];

	request += "\n";
	return request;
}

/**
 * @param   {string}    request     GTP command
 * @returns {string}    response    string containing GTP response in case of
 * success or GTP error in case of failure
 */
string gtp::take_request(string request)
{
	vector<string> result;
	std::istringstream iss(request);
	for (string request; iss >> request;)
		result.push_back(request);
	// To Do
	// assign command and arg
	// string command = result[0];
	// Check if there is an id
	vector<string> args(result.begin() + 1, result.end());

	// call corresponding functions
}
