#include "gtp.h"
using namespace gtp;

// Core Play Commands

/**
 * @param   {Move}  move  a move (Color and vertex) to play
 * @returns {void}
 */
void GTPEngine::play(Move move)
{
    uint32_t row = 0;
    uint32_t column = 0;
    Vertex::indices(move.vertex, row, column);
    uint32_t index = go::engine::BoardState::index(BOARD_SIZE-row, column-1);

    auto game_state = game.get_game_state();
    go::engine::Action agent_action = {index, game_state.player_turn};
    game.make_move(agent_action);
}

/**
 * @param   {Color}         color   Color for which to generate a move
 * @returns {Vertex|string} vertex  Vertex where the move was played or the
 * string \resign"
 */
std::string GTPEngine::genmove(Color color)
{
	auto game_state = game.get_game_state();
	auto agent = (game_state.player_turn == 0) ? agent1 : agent2;
	
	uint32_t move = agent->generate_move(game);
	go::engine::Action agent_action = {move, game_state.player_turn};

	// accept the move only if played in time
    std::string result = "pass";
	if (game.make_move(agent_action))
		result = (move != go::engine::Action::PASS) ? Vertex(get_alphanumeric_position(move)).val() : result;

	return result;
}

/**
 * @param   none
 * @returns {void}
 */
void GTPEngine::undo()
{
}
