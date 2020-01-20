#include "engine/board.h"
#include "gtp/gtp.h"
#include <memory>
#include <iostream>

using namespace go::engine;
using namespace go;

int main()
{
	go::gtp::GTPController{}.main_loop();
}
