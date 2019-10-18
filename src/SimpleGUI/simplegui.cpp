#include "simplegui.h"
using namespace std;

namespace go
{
    namespace simplegui{

        BoardSimpleGUI::BoardSimpleGUI(){initializeBoard();};

        // initialize board at start of the game;
        void BoardSimpleGUI::initializeBoard(){
            memset(board,'.',sizeof board);
        }

        // clears console
        void BoardSimpleGUI::clearScreen(){
            // Hacky solution, but crossplatform :D
            cout << "\x1B[2J\x1B[H";
        }

        // prints board to console
        void BoardSimpleGUI::printBoard(){
            clearScreen();
            for(int i=0;i<BOARD_SIZE;++i){
                cout<<"\t\t";
                for(int j=0;j<BOARD_SIZE;++j)
                    cout<<board[i*BOARD_SIZE+j]<<" \n"[j==BOARD_SIZE-1];
            }
        }

        // changes board based on action, points are 1-based
        void BoardSimpleGUI::makeMove(int x,int y,bool player){

            //input is 1-based
            --x,--y;

            assert(x >= 0);
            assert(y >= 0);
            assert(x < BOARD_SIZE);
            assert(y < BOARD_SIZE);

            x = BOARD_SIZE - 1 - x;

            board[x*BOARD_SIZE+y] = (!player?'@':'#');
        }

    }
}
