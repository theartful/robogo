#ifndef GTP_GTP_H_
#define GTP_GTP_H_

#include <unordered_map>
#include <variant>

#include "engine/board.h"
#include "gtp/utility.h"

namespace go::gtp
{

struct Vertex
{
	uint32_t row;
	uint32_t col;

	explicit Vertex(uint32_t row_ = 0, uint32_t col_ = 0) : row(row_), col(col_)
	{
	}
};

enum class Color
{
	BLACK,
	WHITE
};

struct GTPCommand
{
	std::string_view id;
	std::string_view command;
	std::vector<std::string_view> args;

	explicit GTPCommand(
	    std::string_view id_ = {}, std::string_view command_ = {},
	    std::vector<std::string_view> args_ = {})
	    : id{id_}, command{command_}, args{std::move(args_)}
	{
	}
};

struct GTPCommandResult
{
	std::string output;
	bool error;

	explicit GTPCommandResult(std::string output_ = {}, bool error_ = false)
	    : output(output_), error(error_)
	{
	}
};

using GTPFunction = std::function<GTPCommandResult(const GTPCommand&)>;

class GTPController
{
public:
	GTPController();
	void main_loop();

	// supported commands
	uint32_t protocol_version();
	std::string_view name();
	std::string_view version();
	bool known_command(std::string_view command);
	std::string list_commands();

	void quit();
	expected<bool> boardsize(uint32_t size);
	void komi(float new_komi);
	void clearboard();
	expected<void> play(Color color, Vertex vertex);
	Vertex genmove(Color color);
	std::string showboard();

private:
	template <typename R, typename... Args>
	GTPFunction to_gtp_function(R (GTPController::*func)(Args...));

private:
	using GTPFunctionMap = std::unordered_map<std::string_view, GTPFunction>;

	engine::GameState game;
	engine::Rules rules;
	GTPFunctionMap function_map;
	std::vector<std::string_view> known_commands;
};

} // namespace go::gtp

#endif // GTP_GTP_H_