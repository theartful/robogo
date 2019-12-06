#ifndef GTP_H_
#define GTP_H_

#include "entities.h"
#include "AdminstrativeCommands.cpp"
// #include "SetupCommands.cpp"
// #include "CorePlayCommands.cpp"
// #include "TournamentCommands.cpp"
// #include "RegressionCommands.cpp"
// #include "DebugCommands.cpp"

// Adminstrative Commands
uint32_t protocol_version();
List<string> name();
List<string> version();
bool known_command(string command_name);
MultiLineList<string> list_commands();
void quit();

// Setup Commands
void boardsize(uint32_t size);
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
void time_settings(uint32_t main_time, uint32_t byo_yomi_time, uint32_t byo_yomi_stones);
void time_left(Color color, uint32_t time, uint32_t stones);
string final_score();
MultiLineList<List<Vertex>> final_status_list(string status);

// Regression Commands
void loadsgf(string filename, uint32_t move_number);
Alternative<Vertex, string> reg_genmove(Color color);

// Debug Commands
MultiLineList<List<string>> showboard();

// Handle Requests
string makeRequest(string command,vector<string> args,int id);
string takeRequest(string request);

#endif
