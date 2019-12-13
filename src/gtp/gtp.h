#ifndef GTP_GTP_H_
#define GTP_GTP_H_

#include "controller/agent.h"
#include "controller/game.h"
#include "entities.h"
#include <map>
#include <utility>
namespace gtp
{
class GTPEngine
{
public:
	static const uint32_t BOARD_SIZE = 19;
	static const uint32_t NUM_COMMANDS = 16U;
	static const std::string commands[];
	static const std::map<std::string, uint32_t> commands_args;

	GTPEngine();
	void game_loop();

	// Adminstrative Commands
	static uint32_t protocol_version();
	static List<string> name();
	static List<string> version();
	static Boolean known_command(string command_name);
	static MultiLineList<string> list_commands();
	void quit();

	// Setup Commands
	void boardsize(uint32_t size);
	void clear_board();
	void komi(float new_komi);

	// Core Play Commands
	void play(Move move);
	std::string genmove(Color color);
	void undo();

	// Tournament Commands
	string final_score();
	MultiLineList<List<Vertex>> final_status_list(string status);

	// Debug Commands
	inline bool is_special(uint32_t idx);
	char get_board_symbol(go::engine::Cell cell, uint32_t x, uint32_t y);
	std::string showboard();

	static std::pair<uint32_t, std::vector<std::string>> tokenize(string str);
	static std::string parse_request(
	    std::string request, std::string& error_prefix,
	    std::string& response_prefix, std::string& command,
	    std::vector<std::string>& args);
	static std::pair<uint32_t, std::string> parse_response(string response);

	static std::string
	make_request(string command, vector<string> args, uint32_t id);
	std::string take_request(string request);

private:
	go::Game game;
	std::shared_ptr<go::Agent> agent1;
	std::shared_ptr<go::Agent> agent2;

	void to_lower(std::string& s);
	static std::string get_alphanumeric_position(uint32_t x, uint32_t y);
	static std::string get_alphanumeric_position(uint32_t pos);
};
} // namespace gtp

#endif
