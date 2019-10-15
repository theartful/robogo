#include <CppUTest/CommandLineTestRunner.h>
#include "./sample.cpp"

int main(int ac, char** av)
{
    return CommandLineTestRunner::RunAllTests(ac, av);
}
