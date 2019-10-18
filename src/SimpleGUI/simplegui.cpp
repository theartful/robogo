#include "simplegui.h"

go::simplegui::BoardSimpleGUI::BoardSimpleGUI()
{
	initializeBoard();
}

BoardSimpleGUI::BoardSimpleGUI(){initializeBoard();}

// clears console
void go::simplegui::BoardSimpleGUI::clear_screen(){
    // Hacky solution, but crossplatform :D
    std::cout << "\x1B[2J\x1B[H";
}

// prints board to console

inline char get_board_symbol(Cell cell){
    switch(cell){
        case WHITE:
            return '@';
            break;
        case BLACK:
            return '#';
            break;
        default:
            return '.';
    }
}
void go::simplegui::BoardSimpleGUI::print_board(const BoardState& board){
    for(int i=0;i<BOARD_SIZE;++i){
        std::cout<<"\t\t";
        for(int j=0;j<BOARD_SIZE;++j)
            std::cout<<get_board_symbol(board(i,j))<<" \n"[j==BOARD_SIZE-1];
    }
}

void go::simplegui::print_game_state(const GameState& game_state){
    clear_screen();
    print_board(game_state.board_state);
    std::cout<<"Players turn: "<<game_state.number_played_moves<<"\t\t\t";
    std::cout<<"Number of played moves: "<<game_state.number_played_moves<<endl;

    std::cout<<"\tPlayer 0 \t\t\t Player 1"<<endl;

    std::cout<<"Number of Captured: "<<game_state.players[0].number_captured_enemies;
    std::cout<<"\t\t\tNumber of Captured: "<<game_state.players[1].number_captured_enemies<<endl;

    std::cout<<"Number of Alive: "<<game_state.players[0].number_alive_stones;
    std::cout<<"\t\t\tNumber of Alive: "<<game_state.players[1].number_alive_stones<<endl;

<<<<<<< HEAD
    board[x*BOARD_SIZE+y] = (!player?'@':'#');
}
=======
    std::cout<<"Total Score: "<<game_state.players[0].total_score;
    std::cout<<"\t\t\t\tTotal Score: "<<game_state.players[1].total_score<<endl;
}
>>>>>>> Add simple gui
