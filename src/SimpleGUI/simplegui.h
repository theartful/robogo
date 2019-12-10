#ifndef SRC_SIMPLEGUI_SIMPLEGUI_H_
#define SRC_SIMPLEGUI_SIMPLEGUI_H_
#include "controller/agent.h"
#include "controller/game.h"
#include "engine/board.h"
#include "engine/cluster.h"
#include <string>

namespace go
{

namespace simplegui
{

class BoardSimpleGUI : public Agent
{
public:
	BoardSimpleGUI();

	virtual uint32_t generate_move(const Game& game) override;

	// prints board to console
	static void
	print_board(const go::engine::BoardState& board, uint32_t player_turn);

	// transforms x and y to alphanumeric position
	static std::string get_alphanumeric_position(uint32_t x, uint32_t y);
	static std::string get_alphanumeric_position(uint32_t pos);

	// utility function to get symbol on board
	static char get_board_symbol(go::engine::Cell cell, uint32_t x, uint32_t y);
	
	std::vector<go::engine::Action> read_moves(const go::engine::GameState& game_state);

private:
	static const uint32_t BOARD_SIZE = 19;

	// clears console
	static void clear_screen();

	// utility to transform data read into x and y
	void read_position(uint32_t& x, uint32_t& y);

	// transforms position from alphanumeric to x and y
	void get_index(char column, uint32_t row, uint32_t& x, uint32_t& y);

	// prints info of game state (Board, turn, players score)
	void print_game_state(const Game& game);

	// prints cluster liberties
	void print_liberties(const go::engine::Cluster& cluster);

	// prints cluster information
	void print_cluster_info(
	    uint32_t index, const go::engine::ClusterTable& table,
	    const go::engine::BoardState& state);

	// checks if index is key point
	static inline bool is_special(uint32_t idx);

	// transforms string into lower case
	static void to_lower(std::string& str);
};

} // namespace simplegui
} // namespace go

#endif // SRC_SIMPLEGUI_SIMPLEGUI_H_
