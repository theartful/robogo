#include "gtp/gtp.h"

#include <iostream>

using namespace go::engine;
using namespace go;

int main()
{
	go::gtp::GTPController{}.main_loop(std::cout, std::cin);
}
