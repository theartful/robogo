#ifndef GTP_GTP_H_
#define GTP_GTP_H_

#include "entities.h"
#include <map>
// #include "AdminstrativeCommands.cpp"
// #include "SetupCommands.cpp"
// #include "CorePlayCommands.cpp"
// #include "TournamentCommands.cpp"
// #include "RegressionCommands.cpp"
// #include "DebugCommands.cpp"

namespace gtp
{
std::map<string, int> commandsList = {{"protocol_version", 0},
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
// void boardsize(uint32_t size);
void clear_board();
void komi(float new_komi);
// List<Vertex> fixed_handicap(uint32_t number_of_stones);
// List<Vertex> place_free_handicap(uint32_t number_of_stones);
// List<Vertex> set_free_handicap(uint32_t number_of_stones);

// Core Play Commands
void play(Move move);
Alternative<Vertex, string> genmove(Color color);
void undo();

// Tournament Commands
// void time_settings(uint32_t main_time, uint32_t byo_yomi_time, uint32_t byo_yomi_stones);
// void time_left(Color color, uint32_t time, uint32_t stones);
string final_score();
MultiLineList<List<Vertex>> final_status_list(string status);

// Regression Commands
void loadsgf(string filename, uint32_t move_number);
// Alternative<Vertex, string> reg_genmove(Color color);

// Debug Commands
MultiLineList<List<string>> showboard();

string make_request(string command, vector<string> args, int id = -1);
string take_request(string request);
};

#endif
