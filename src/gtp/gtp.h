#ifndef GTP_GTP_H_
#define GTP_GTP_H_

#include <unordered_map>
#include <tuple>
#include <functional>

#include "engine/board.h"
#include "engine/game.h"
#include "gtp/utility.h"

namespace go::gtp
{

struct Vertex
{
	uint32_t row;
	uint32_t col;

	explicit Vertex(uint32_t row_, uint32_t col_) :
		row(row_), col(col_)
	{
	}

	explicit Vertex(uint32_t pos)
	{
		std::tie(row, col) = go::engine::BoardState::row_col(pos);
		// we count rows from 1 not 0
		row++;
	}

	Vertex() {}

	constexpr uint16_t index() const
	{
		return go::engine::BoardState::index(row, col);
	}
};

enum class Color
{
	Black,
	White,
	Empty,
};

inline uint8_t to_player_idx(Color color)
{
	return color == Color::Black? uint8_t{0} : uint8_t{1};
}

inline go::engine::Action to_action(Color color, const Vertex& vertex)
{
	return go::engine::Action{vertex.index(), to_player_idx(color)};
}

struct Score
{
	float black;
	float white;
};

struct GTPCommand
{
	std::string_view id;
	std::string_view command;
	std::vector<std::string_view> args;

	explicit GTPCommand(
		std::string_view id_ = {}, std::string_view command_ = {},
		std::vector<std::string_view> args_ = {}) :
			id{id_}, command{command_}, args{std::move(args_)}
	{
	}
};

struct GTPCommandResult
{
	std::string output;
	bool error;

	explicit GTPCommandResult(std::string output_ = {}, bool error_ = false) :
		output(output_), error(error_)
	{
	}
};

using GTPFunction = std::function<GTPCommandResult(const GTPCommand&)>;

class GTPController
{
public:
	GTPController();
	void main_loop(std::ostream& out, std::istream& in);
	void handle_request(std::string request, std::ostream& out);

	// supported commands
	uint32_t protocol_version();
	std::string_view name();
	std::string_view version();
	bool known_command(std::string_view command);
	std::string list_commands();

	void quit();
	expected<void> boardsize(uint32_t size);
	uint32_t query_boardsize();
	void komi(float new_komi);
	expected<void> play(Color color, Vertex vertex);
	Vertex genmove(Color color);
	std::string showboard();
	void clear_board();

	// debugging & testing
	std::string rg_showboard();
	uint32_t countlib(Vertex vertex);
	std::vector<Vertex> findlib(Vertex vertex);
	Color color(Vertex vertex);
	bool is_legal(Color color, Vertex vertex);
	bool is_suicide(Color color, Vertex vertex);
	bool is_ko(Color color, Vertex vertex);
	uint32_t captures(Color color);
	Score final_score();
	Color loadsgf(std::string file, int32_t move_num);

private:
	template <typename R, typename... Args>
	GTPFunction to_gtp_func(R (GTPController::*func)(Args...));

private:
	using GTPFunctionMap = std::unordered_map<std::string_view, GTPFunction>;

	engine::GameState game;
	engine::Rules rules;
	GTPFunctionMap function_map;
	std::vector<std::string_view> known_commands;
	bool quit_flag;
};

} // namespace go::gtp

#endif // GTP_GTP_H_
