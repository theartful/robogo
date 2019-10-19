#ifndef SRC_SIMPLEGUI_SIMPLEGUI_H_
#define SRC_SIMPLEGUI_SIMPLEGUI_H_

namespace go{

<<<<<<< HEAD
namespace simplegui{

struct BoardSimpleGUI
{
    static const int BOARD_SIZE = 19;

    BoardSimpleGUI();

    // clears console
    void clear_screen();

    // prints info of game state (Board, turn, players score)
    void print_game_state(const GameState& game_state);

    //print board to console, requires clearing console
    void print_board(const BoardState& board);

=======
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
>>>>>>> 8293200... Fix linter errors

};

}// namespace simplegui
}// namespace go

#endif  // SRC_SIMPLEGUI_SIMPLEGUI_H_
