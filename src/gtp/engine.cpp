#include "gtp.h"
#include "agents/mcts_agent.h"
using namespace gtp;

GTPEngine::GTPEngine()
{
	agent1 = std::make_shared<go::MCTSAgent>();
	agent2 = std::make_shared<go::MCTSAgent>();
	agent1->set_player_idx(0);
	agent2->set_player_idx(1);
	game.register_agent(agent1, 0);
	game.register_agent(agent2, 1);
}

void GTPEngine::game_loop()
{
	std::string request;
	while (!game.is_game_finished())
	{
		std::getline(std::cin, request);
		to_lower(request);
		std::cout << take_request(request);
	}
}

void GTPEngine::to_lower(std::string& str)
{
	for_each(str.begin(), str.end(), [](char& c) { c = std::tolower(c); });
}

std::string GTPEngine::get_alphanumeric_position(uint32_t x, uint32_t y)
{
	uint32_t row = BOARD_SIZE - x;
	char column = 'A' + y;
	if (column >= 'I')
		column++;
	return column + std::to_string(row);
}

std::string GTPEngine::get_alphanumeric_position(uint32_t pos)
{
	return get_alphanumeric_position((pos / 21) - 1, pos % 21 - 1);
}
