#include "test_loader.h"
#include "gtp/gtp.h"
#include <algorithm>
#include <cctype>
#include <fstream>
#include <gtest/gtest.h>
#include <regex>
#include <sstream>
#include <utility>

TestLoader::TestLoader(std::string file_) : file{std::move(file_)}
{
}

void TestLoader::run_test()
{
	go::gtp::GTPController gtp_controller{};

	std::string last_response;
	std::string last_command;
	std::ifstream fin(file);
	uint32_t line_num = 1;
	for (std::string line; std::getline(fin, line); line = {}, line_num++)
	{
		if (line.size() <= 1)
		{
			continue;
		}
		else if (line[0] == '#' && line[1] == '?')
		{
			auto first_nonspace = std::distance(
				line.begin(),
				std::find_if(line.begin() + 2, line.end(), [](unsigned char c) {
					return !std::isspace(c);
				}));
			std::string re_str =
				line.substr(static_cast<size_t>(first_nonspace));
			std::regex re{re_str};
			EXPECT_TRUE(std::regex_search(last_response, re))
				<< "Test case at line " << line_num << " failed."
				<< "Output of command \"" << last_command
				<< "\" is \"" << last_response << "\". Expected to match \""
				<< re_str << "\"!";
		}
		else if (line[0] == '#')
		{
			continue;
		}
		else
		{
			std::ostringstream ss;
			gtp_controller.handle_request(line, ss);
			last_response = ss.str();
			last_response.erase(
				std::remove(last_response.begin(), last_response.end(), '\n'),
				last_response.end());
			last_command = std::move(line);
		}
	}
}
