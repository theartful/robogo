#include "includes/catch.hpp"

#include "SGF _GNU library/SGFinterface.h"
#include "controller/agent.h"
#include "controller/game.h"
#include "engine/board.h"
#include "engine/cluster.h"
#include "engine/interface.h"
#include <fstream>
using namespace go;
using namespace go::engine;

TEST_CASE("Invalid move due to existance of Ko ")
{
	Game game;
	bool all = true;
	char const* filename = "sgfFiles/ko.sgf";
	SGFNode* treeHead = readsgffile(filename);
	std::vector<Action> actions = load_sgf_tree(treeHead);
	for (unsigned int i = 0; i < actions.size(); i++)
	{
		if ((i == 6) || (i == 13) || (i == 15) || (i == 24) || (i == 34))
			all = all && !(game.make_move(actions[i]));
		else
			all = all && game.make_move(actions[i]);
	}
	REQUIRE(!game.is_game_finished());
	REQUIRE(all);
	REQUIRE(actions.size() == 35);
}


TEST_CASE ("execution of pass correctly "){
    
    Game game;
	bool all = true;
	char const* filename = "sgfFiles/passTurn.sgf";
	SGFNode* treeHead = readsgffile(filename);
	std::vector<Action> actions = load_sgf_tree(treeHead);
    BoardState boardState;
	for (unsigned int i = 0; i < actions.size(); i++)
	{  
		all = all && game.make_move(actions[i]);
        if (i == 3)
            boardState = game.get_board_state();
	}
	REQUIRE(all);
	REQUIRE(!game.is_game_finished());
    for (unsigned int i = 0; i < 19 ; i++)
        for (unsigned int j = 0 ; j < 19 ;j++){
            uint32_t place = BoardState::index(i, j);
            REQUIRE (game.get_board_state().board[place] == boardState.board[place]);
        }
}


TEST_CASE("End of Game due to 2 Passes ")
{
	
	Game game;
	bool all = true;
	char const* filename = "sgfFiles/endGame.sgf";
	SGFNode* treeHead = readsgffile(filename);
	std::vector<Action> actions = load_sgf_tree(treeHead);

	for (unsigned int i = 0; i < actions.size(); i++)
	{
		all = all && game.make_move(actions[i]);

	}
	REQUIRE(all);
	REQUIRE(game.is_game_finished());
}


TEST_CASE("alternating turns between players ")
{
    bool all = true;
    Game game ;
    char const* filename = "sgfFiles/turn.sgf";
    SGFNode* treeHead = readsgffile(filename);
    std::vector<Action> actions = load_sgf_tree(treeHead);
    REQUIRE (game.get_game_state().player_turn == 0);
    for (unsigned int i = 0; i < actions.size(); i++)
    {
        if (i % 2 == 0)
            REQUIRE (game.get_game_state().player_turn == 0);
        else 
            REQUIRE (game.get_game_state().player_turn == 1);

        all = all && game.make_move(actions[i]);
    }
    REQUIRE(!game.is_game_finished());
    REQUIRE(all);
    
}


TEST_CASE("Check score ")
{
    
}


TEST_CASE("Check Capture for each player")
{
	std::ofstream myfile("out2.txt");
	bool all = true;
	char const* filename;
	SGFNode* treeHead;
    std::vector<Action> actions;
	SECTION("Capture of only one stone at atime ")
	{
        Game game ;
		filename = "sgfFiles/ko.sgf";
		treeHead = readsgffile(filename);

		actions = load_sgf_tree(treeHead);
		myfile << "REEEEEEEEEEEEEEEEEEEEEEEEEM" << std ::endl;
		myfile << game.get_game_state().players[0].number_captured_enemies << std :: endl;
		myfile << game.get_game_state().players[1].number_captured_enemies << std:: endl;
		for (unsigned int i = 0; i < actions.size(); i++)
		{
			if ((i == 6) || (i == 13) || (i == 15) || (i == 24) || (i == 34))
				all = all && !(game.make_move(actions[i]));
			else
				all = all && game.make_move(actions[i]);
		}
		REQUIRE(!game.is_game_finished());
		REQUIRE(all);
		myfile << "YASSER ASHRAF SALAH"<< std::endl;
		myfile << game.get_game_state().players[0].number_captured_enemies << std::endl;
		myfile << game.get_game_state().players[1].number_captured_enemies << std ::endl;
		REQUIRE(game.get_game_state().players[0].number_captured_enemies == 4);
        REQUIRE(game.get_game_state().players[1].number_captured_enemies == 4);
	}
    
	SECTION("Capture of only more than one stone at time ")
	{
        Game game;
		filename = "sgfFiles/capture.sgf";
		treeHead = readsgffile(filename);
	    actions = load_sgf_tree(treeHead);
		for (unsigned int i = 0; i < actions.size(); i++)
		{
			all = all && game.make_move(actions[i]);
		}
		REQUIRE(!game.is_game_finished());
		REQUIRE(all);
       
		REQUIRE(game.get_game_state().players[0].number_captured_enemies == 3);
        REQUIRE(game.get_game_state().players[1].number_captured_enemies == 3);
        
	}
}

TEST_CASE ("Sucide"){
     //std::ofstream myfile("out.txt");
    Game game;
	bool all = true;
	char const* filename = "sgfFiles/suicide.sgf";
	SGFNode* treeHead = readsgffile(filename);
	std::vector<Action>actions = load_sgf_tree(treeHead);
    bool suicide = true;
    // myfile << actions.size();
	for (unsigned int i = 0; i < actions.size(); i++)
	{
        
        if (i == 22)
        {
            suicide = suicide && is_suicide_move(game.get_cluster_table(),game.get_board_state(),actions[i]);
            all = all && !game.make_move(actions[i]);
        }
        else
        {
            all = all && game.make_move(actions[i]);
            suicide = suicide && !is_suicide_move(game.get_cluster_table(),game.get_board_state(),actions[i]);
        }
        
    }
	REQUIRE(!game.is_game_finished());
	REQUIRE(all);
	REQUIRE(suicide);
}




TEST_CASE("Board moves successfully dismiss Invalid moves ")
{ 
	Game game;
	bool all = true;
	char const* filename = "sgfFiles/validGame.sgf";
	SGFNode* treeHead = readsgffile(filename);
	std::vector<Action> actions = load_sgf_tree(treeHead);

	for (unsigned int i = 0; i < actions.size(); i++)
	{
		all = all && game.make_move(actions[i]);
	}
	REQUIRE(all);
	REQUIRE(!game.is_game_finished());
}


TEST_CASE("Board updates")
{
	Game game;
	BoardState state;

	SECTION("checking correct placement of cell")
	{
		uint32_t place = BoardState::index(0, 2);
		state.board[place] = Cell::WHITE; // unit functionality assumed correct
		Action action = {place, 1};
		REQUIRE(game.make_move(action));
		REQUIRE(game.get_board_state().board[place] == state.board[place]);
		REQUIRE(!game.is_game_finished());
	}
}
