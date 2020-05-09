#ifndef _TESTS_TEST_LOADER_H_
#define _TESTS_TEST_LOADER_H_

#include <string>

class TestLoader
{
public:
	TestLoader(std::string file_);
	void run_test();

private:
	std::string file;

};

#endif // _TESTS_TEST_LOADER_H_
