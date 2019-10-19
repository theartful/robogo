#ifndef SRC_SIMPLEGUI_SIMPLEGUI_H_
#define SRC_SIMPLEGUI_SIMPLEGUI_H_

#include <iostream>
#include <cassert>
#include <cstring>

namespace go{

namespace simplegui{

struct BoardSimpleGUI
{
    
    static const int BOARD_SIZE = 19;
    char board[BOARD_SIZE*BOARD_SIZE];

    BoardSimpleGUI();

    // initialize board at start of the game;
    void initializeBoard();

    // clears console
    void clearScreen();

    // prints board to console
    void printBoard();

    // changes board based on action, points are 1-based
    void makeMove(int x,int y,bool player);

};

}// namespace simplegui
}// namespace go

#endif  // SRC_SIMPLEGUI_SIMPLEGUI_H_
