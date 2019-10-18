#include <iostream>
#include "simplegui.h"

using namespace std;


int main(){

    go::simplegui::BoardSimpleGUI gui;
    
    bool turn = false;

    while(true){
        gui.printBoard();

        cout<<"Enter x y: ";
        int x,y;

        cin>>x>>y;

        gui.makeMove(x,y,turn);

        turn = !turn;

    }
}