#include "engine/engine.h"
#include "engine/game.h"
#include "sgf.h"

#include <cstdlib>
#include <iostream>

namespace go::sgf
{

static uint32_t get_uint_property(const SGFProperty& property)
{
	return static_cast<uint32_t>(std::atoi(property.value.c_str()));
}

static float get_float_property(const SGFProperty& property)
{
	return static_cast<float>(std::atof(property.value.c_str()));
}

static uint32_t
get_move_property(const SGFProperty& property, const engine::GameState& game)
{
	if (property.value.size() < 2)
		return engine::Action::PASS;
	uint32_t row = static_cast<uint32_t>(std::toupper(property.value[1]) - 'A');
	uint32_t col = static_cast<uint32_t>(std::toupper(property.value[0]) - 'A');
	if (row >= game.board.size || col >= game.board.size)
		return engine::Action::PASS;
	else
		return engine::BoardState::index(18 - row, col);
}

void execute_sgf(
	const std::shared_ptr<SGFNode>& tree, engine::GameState& game,
	engine::Rules& rules, int32_t until)
{
	if (!tree)
		return;

	rules = {};
	int32_t move_num = 1;
	for (auto crawler = tree;;)
	{
		for (auto& prop : crawler->props)
		{
			switch (prop.name)
			{
			case SGFPropertyName::Size: {
				game = engine::GameState{get_uint_property(prop)};
				break;
			}
			case SGFPropertyName::Komi: {
				rules.komi = get_float_property(prop);
				break;
			}
			case SGFPropertyName::AddBlack:
			case SGFPropertyName::AddWhite:
			case SGFPropertyName::BlackMove:
			case SGFPropertyName::WhiteMove: {
				uint32_t move = get_move_property(prop, game);
				uint32_t player_idx;
				if (prop.name == SGFPropertyName::AddBlack ||
					prop.name == SGFPropertyName::BlackMove)
					player_idx = 0;
				else
					player_idx = 1;
				engine::force_move(game, engine::Action{move, player_idx});
				move_num++;
				break;
			}
			case SGFPropertyName::StartColor: {
				exit(1);
				break;
			}
			default: {
				break;
			}
			}
		}
		if (!crawler->children.empty() && move_num != until)
			crawler = crawler->children.front();
		else
			break;
	}
} // namespace go::sgf

} // namespace go::sgf
