#include <filesystem>
#include <gtest/gtest.h>
#include <string>

#include "test_loader.h"

class InterfaceTest : public testing::TestWithParam<std::string>
{
};

static std::vector<std::string> get_scripts()
{
	std::vector<std::string> scripts;
	std::string path = "./scripts/";
	for (const auto& entry : std::filesystem::directory_iterator(path))
		scripts.push_back(entry.path());
	return scripts;
}

TEST_P(InterfaceTest, script)
{
	TestLoader loader{GetParam()};
	loader.run_test();
}

INSTANTIATE_TEST_SUITE_P(
	Script, InterfaceTest, testing::ValuesIn(get_scripts()));
