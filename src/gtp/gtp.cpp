#include "gtp.h"
#include "entities.h"
#include <iostream>
#include<string>
#include<sstream>
#include <iterator>
#include <algorithm>
#include <map>
#include <functional>
using namespace std;

map <string,int> commands= {
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

  // #define protocol_version function<uint32_t()> protocol_version();
  // #define name function<List<string>()>
  // #define version function<List<string>()>
  // #define known_command function<int(int)>
  // #define list_commands function<int(int)>
  // #define quit function<int(int)>
  // #define boardsize function<int(int)>
  // #define clear_board function<int(int)>
  // #define komi function<int(int)>
  // #define fixed_handicap function<int(int)>
  // #define place_free_handicap function<int(int)>
  // #define set_free_handicap function<int(int)>
  // #define play function<int(int)>
  // #define genmove function<int(int)>
  // #define undo function<int(int)>
  // #define time_settings function<int(int)>
  // #define time_left function<int(int)>
  // #define final_score function<int(int)>
  // #define final_status_list function<int(int)>
  // #define loadsgf function<int(int)>
  // #define reg_genmove function<int(int)>
  // #define showboard function<int(int)>
/**
 * @param   {string}    command Command Name
 * @param   {Array}     args    Array of objects that contains command arguments
 * @returns {string}    request string containing required request
 */

 string makeRequest(string command,vector<string> args,int id=-1) {
   // Validate Command
   if (!known_command(command))
   {
     return "Invalid Argument: command doesn't exist";
   }

   // Validate id
   if(id <= 0 && id != -1)
   {
     return "Invalid Argument: id must be a positive integer";
   }

   // Validate arguments
   int required_args = commands[command];
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

 /**
  * @param   {string}    request     GTP command
  * @returns {string}    response    string containing GTP response in case of success or GTP error in case of failure
  */
  string takeRequest(string request) {
    vector<string> result;
    istringstream iss(request);
    int id = NULL;
    string command;
    vector<string> args;
    try
    {
      for(string request; iss >> request; )
      {
        result.push_back(request);
      }

    // To Do
    // assign command and arg

    // Check if there is an id

    try
    {
      id = stoi(result[0]);
      command = result[1];
      args = vector<string>(result.begin()+2,result.end());

      // Validate ID
      if(id <= 0)
      {
        return "Invalid Argument: id must be a positive integer";
      }
    }
    catch(std::invalid_argument& e)
    {
      command = result[0];
      args = vector<string>(result.begin()+1,result.end());
    }
    // cout<<id<<endl<<command<<endl;
  }
  catch(exception & ex)
  {
    return "? "+(string)ex.what()+"\n\n";
  }

  // Validate Command
  if(!known_command(command))
  {
    string id_ = (id) ? to_string(id)+" " : " ";
    return "?"+id_+"command doesn't exist\n\n";
  }

  // Validate arguments
  int required_args = commands[command];
  int given_args = args.size();
  if(given_args != required_args)
  {
    return "Invalid Argument: "+ command + " arguments list should be " + to_string(required_args) + ", however, " + to_string(given_args) + " was provided";
  }

return "";

    // call corresponding functions
  }
 int main()
 {
   // vector<string> args = {"w","a15"};
   // vector<string> args ;
   // string request = makeRequest("protocol_version",args);
   string request = "5 protocol_version";
   string result = takeRequest(request);
   // cout<<request<<endl;
   cout<<result<<endl;
   return 0;
 }
