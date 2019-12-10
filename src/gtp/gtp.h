#ifndef GTP_GTP_H_
#define GTP_GTP_H_

#include "entities.h"
#include <map>
#include <utility>
namespace gtp
{
static const string commands[] = {"protocol_version",
                           "name",
                           "version",
                           "known_command",
                           "list_commands",
                           "quit",
                           "clear_board",
                           "komi",
                           "play",
                           "genmove",
                           "undo",
                           "final_score",
                           "final_status_list",
                           "showboard",
                           "setboard"};
static const std::map<string, int> commands_args = {{"protocol_version", 0},
                                             {"name", 0},
                                             {"version", 0},
                                             {"known_command", 1},
                                             {"list_commands", 0},
                                             {"quit", 0},
                                             {"clear_board", 0},
                                             {"komi", 1},
                                             {"play", 1},
                                             {"genmove", 1},
                                             {"undo", 0},
                                             {"final_score", 0},
                                             {"final_status_list", 1},
                                             {"showboard", 0},
                                             {"setboard", 1}};

// Adminstrative Commands
uint32_t protocol_version();
List<string> name();
List<string> version();
Boolean known_command(string command_name);
MultiLineList<string> list_commands();
void quit();

// Setup Commands
void clear_board();
void komi(float new_komi);

// Core Play Commands
void play(Move move);
Alternative<Vertex, string> genmove(Color color);
void undo();

// Tournament Commands
string final_score();
MultiLineList<List<Vertex>> final_status_list(string status);

// Debug Commands
MultiLineList<List<string>> showboard();

std::string make_request(string command, vector<string> args, uint32_t id);
std::pair<uint32_t, string> parse_response(string response);
std::string take_request(string request);
} // namespace gtp

#endif
