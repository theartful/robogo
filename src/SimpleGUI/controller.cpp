#include "simplegui.h"
#include <string>
#include <iostream>
#include <sstream>
#include <algorithm>

int main(){

    std::string user_input;
    go::simplegui::BoardSimpleGUI gui;
    std::stringstream stream;

    std::string command;
    int x,y;

    while(true){
        gui.get_user_input(user_input);
        stream.clear();
        stream << user_input;
        stream >> command;
        
        for_each(command.begin(),command.end(),[] (char& c){
            c = std::tolower(c);
        });

        if(command== "disp"){
            // gui.print_game_state(game_state);
        }
        else if(command == "mv"){
            stream>>x>>y;
            // engine.make_move(x,y)
            // gui.print_game_state(game_state);
        }
        else if(command == "lib"){
            stream>>x>>y;
            // cluster = get_cluster(x,y)
            // gui.print_cluster_info(cluster);
        }
        else{
            // gui.print_game_state(game_state);
        }
    }
}