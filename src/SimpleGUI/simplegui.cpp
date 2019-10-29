#include "simplegui.h"

#include <iostream>
#include <cassert>
#include <cstring>

go::simplegui::BoardSimpleGUI::BoardSimpleGUI(){}

// clears console
void go::simplegui::BoardSimpleGUI::clear_screen(){
    // Hacky solution, but crossplatform :D
    std::cout << "\x1B[2J\x1B[H";
}

// helper function to get symbol for players
inline char go::simplegui::BoardSimpleGUI::get_board_symbol(go::engine::Cell cell){
    switch(cell){
        case go::engine::Cell::WHITE:
            return '@';
            break;
        case go::engine::Cell::BLACK:
            return '#';
            break;
        default:
            return '.';
    }
    return '.';
}

// prints board to console
void go::simplegui::BoardSimpleGUI::print_board(const go::engine::BoardState& board){
    for(int i=0;i<BOARD_SIZE;++i){
        std::cout<<"\t\t";
        for(int j=0;j<BOARD_SIZE;++j)
            std::cout<<get_board_symbol(board(i,j))<<" \n"[j==BOARD_SIZE-1];
    }
}

void go::simplegui::BoardSimpleGUI::print_game_state(const go::engine::GameState& game_state){
    clear_screen();
    print_board(game_state.board_state);
    std::cout<<"Players turn: "<<game_state.number_played_moves<<"\t\t\t";
    std::cout<<"Number of played moves: "<<game_state.number_played_moves<<std::endl;

    std::cout<<"\tPlayer 0 \t\t\t Player 1"<<std::endl;

    std::cout<<"Number of Captured: "<<game_state.players[0].number_captured_enemies;
    std::cout<<"\t\t\tNumber of Captured: "<<game_state.players[1].number_captured_enemies<<std::endl;

    std::cout<<"Number of Alive: "<<game_state.players[0].number_alive_stones;
    std::cout<<"\t\t\tNumber of Alive: "<<game_state.players[1].number_alive_stones<<std::endl;

    std::cout<<"Total Score: "<<game_state.players[0].total_score;
    std::cout<<"\t\t\t\tTotal Score: "<<game_state.players[1].total_score<<std::endl;
}