#ifndef GTP_GTP_H_
#define GTP_GTP_H_

#include "entities.h"
#include <map>
#include <utility>
namespace gtp
{
const string commands[] = {"protocol_version",
                           "name",
                           "version",
                           "known_command",
                           "list_commands",
                           "quit",
                           "boardsize",
                           "clear_board",
                           "komi",
                           "fixed_handicap",
                           "place_free_handicap",
                           "set_free_handicap",
                           "play",
                           "genmove",
                           "undo",
                           "time_settings",
                           "time_left",
                           "final_score",
                           "final_status_list",
                           "loadsgf",
                           "reg_genmove",
                           "showboard"};
const std::map<string, int> commands_args = {{"protocol_version", 0},
                                             {"name", 0},
                                             {"version", 0},
                                             {"known_command", 1},
                                             {"list_commands", 0},
                                             {"quit", 0},
                                             {"boardsize", 1},
                                             {"clear_board", 0},
                                             {"komi", 1},
                                             {"fixed_handicap", 1},
                                             {"place_free_handicap", 1},
                                             {"set_free_handicap", 1},
                                             {"play", 1},
                                             {"genmove", 1},
                                             {"undo", 0},
                                             {"time_settings", 3},
                                             {"time_left", 3},
                                             {"final_score", 0},
                                             {"final_status_list", 1},
                                             {"loadsgf", 2},
                                             {"reg_genmove", 1},
                                             {"showboard", 0}};

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

// Regression Commands
void loadsgf(string filename, uint32_t move_number);
Alternative<Vertex, string> reg_genmove(Color color);

// Debug Commands
MultiLineList<List<string>> showboard();

std::string make_request(string command, vector<string> args, uint32_t id);
std::pair<uint32_t, string> parse_response(string response);
std::string take_request(string request);
} // namespace gtp

#endif
