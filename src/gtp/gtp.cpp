#include "gtp.h"
#include "entities.h"
#include <iostream>
#include<string>
#include <iterator>
#include <algorithm>
#include <map>
using namespace std;

map <string,int> commandsList= {
  {"protocol_version",0},
  {"name",0},
  {"version",0},
  {"known_command",1},
  {"list_commands",0},
  {"quit",0},
  {"boardsize",1},
  {"clear_board",0},
  {"komi",1},
  {"fixed_handicap",1},
  {"place_free_handicap",1},
  {"set_free_handicap",1},
  {"play",2},
  {"genmove",1},
  {"undo",0},
  {"time_settings",3},
  {"time_left",3},
  {"final_score",0},
  {"final_status_list",1},
  {"loadsgf",2},
  {"reg_genmove",1},
  {"showboard",0}
 };

/**
 * @param   {string}    command Command Name
 * @param   {Array}     args    Array of objects that contains command arguments
 * @returns {string}    request string containing required request
 */

 string makeRequest(string command,vector<string> args,int id=-1) {
   // Validate Command
   std::map<string,int>::iterator it;
   it = commandsList.find(command);
   if (it == commandsList.end())
   {
     return "Invalid Argument: command doesn't exist";
   }

   // Validate id
   if(id <= 0 && id != -1)
   {
     return "Invalid Argument: id must be a positive integer";
   }

   // Validate arguments
   int required_args = commandsList[command];
   int given_args = args.size();
   if(given_args != required_args)
   {
     return "Invalid Argument: "+ command + " arguments list should be " + to_string(required_args) + ", however, " + to_string(given_args) + " was provided";
   }

   // Costruct request
   string request = "";
   if (id > 0)
   {
     request += to_string(id)+" ";
   }
   request += command;

   // concatenate arguments
   for(int i=0 ; i<args.size() ; i++)
   {
     request += " "+args[i];
   }

   request += "\n";
   return request;

 }
 int main()
 {
   vector<string> args = {"w","a15"};
   string request = makeRequest("play",args);
   cout<<request<<endl;
   return 0;
 }
