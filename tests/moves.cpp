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
	std::ofstream myfile("Out.txt");
	Game game;
	bool all = true;
	char const* filename = "sgfFiles/ko.sgf";
	SGFNode* treeHead = readsgffile(filename);
	std::vector<Action> actions = load_sgf_tree(treeHead);
	
	for (unsigned int i = 0; i < actions.size(); i++)
	{
		
		if ((i == 6) || (i == 13) || (i == 16) || (i == 25) || (i == 35))
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

TEST_CASE("Check passes action"){
	SECTION("End of Game due to 2 Passes ")
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
	SECTION(" check end game due to accumulative pass only ")
	{
		
		Game game;
		bool all = true;
		char const* filename = "sgfFiles/pass.sgf";
		SGFNode* treeHead = readsgffile(filename);
		std::vector<Action> actions = load_sgf_tree(treeHead);

		for (unsigned int i = 0; i < actions.size(); i++)
		{
			all = all && game.make_move(actions[i]);

		}
		REQUIRE(all);
		REQUIRE(!game.is_game_finished());
	}
	
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
	SECTION("Simple score calculation"){
	
	bool all = true;
    Game game ;
    char const* filename = "sgfFiles/score.sgf";
    SGFNode* treeHead = readsgffile(filename);
    std::vector<Action> actions = load_sgf_tree(treeHead);
    REQUIRE (game.get_game_state().player_turn == 0);
    for (unsigned int i = 0; i < actions.size(); i++)
    {
        
        all = all && game.make_move(actions[i]);
    }
    REQUIRE(!game.is_game_finished());
    REQUIRE(all);

	
	REQUIRE(game.get_game_state().players[0].number_captured_enemies == 5);
    REQUIRE(game.get_game_state().players[1].number_captured_enemies == 5);
	
	GameState game_state = game.get_game_state();
	calculate_score(
	    game.get_game_state().board_state, game_state.players[0], game_state.players[1]);
	
	REQUIRE (game_state.players[0].total_score == 40);
	REQUIRE (game_state.players[1].total_score == 45.5);
	}

	SECTION("complicated score calculation"){
	 
	bool all = true;
    Game game ;
    char const* filename = "sgfFiles/score2.sgf";
    SGFNode* treeHead = readsgffile(filename);
    std::vector<Action> actions = load_sgf_tree(treeHead);
    REQUIRE (game.get_game_state().player_turn == 0);
    for (unsigned int i = 0; i < actions.size(); i++)
    {
        
        all = all && game.make_move(actions[i]);
    }
    // REQUIRE(!game.is_game_finished());
    REQUIRE(all);

	
	REQUIRE(game.get_game_state().players[0].number_captured_enemies == 33);
    REQUIRE(game.get_game_state().players[1].number_captured_enemies == 26);
	
	GameState game_state = game.get_game_state();
	calculate_score(
	    game.get_game_state().board_state, game_state.players[0], game_state.players[1]);
	
	REQUIRE (game_state.players[0].total_score == 137);
	REQUIRE (game_state.players[1].total_score == 123.5);
	}
	
    
}


TEST_CASE("Check Capture for each player")
{
	
	bool all = true;
	char const* filename;
	SGFNode* treeHead;
    std::vector<Action> actions;
	SECTION("Capture of only one stone at atime"){
        Game game ;
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
	SECTION("Capture of only more than one stone at time ")
	{
        Game game;
		filename = "sgfFiles/capture2.sgf";
		treeHead = readsgffile(filename);
	    actions = load_sgf_tree(treeHead);
		for (unsigned int i = 0; i < actions.size(); i++)
		{
			all = all && game.make_move(actions[i]);
		}
		REQUIRE(!game.is_game_finished());
		REQUIRE(all);
       
		REQUIRE(game.get_game_state().players[0].number_captured_enemies == 2);
        REQUIRE(game.get_game_state().players[1].number_captured_enemies == 4);

	GameState game_state = game.get_game_state();
	engine::calculate_score(
	    game.get_game_state().board_state, game_state.players[0], game_state.players[1]);

	REQUIRE (game_state.players[0].total_score == 28);
	REQUIRE (game_state.players[1].total_score == 40.5);
        
	}
}

TEST_CASE ("Sucide"){
    
	SECTION(" sucide1"){
		Game game;
		bool all = true;
		char const* filename = "sgfFiles/suicide.sgf";
		SGFNode* treeHead = readsgffile(filename);
		std::vector<Action>actions = load_sgf_tree(treeHead);
	
		//std:: ofstream myfile("out.txt");
		bool suicide = true;
		
		for (unsigned int i = 0; i < actions.size(); i++)
		{
				
			if ((i == 20) || (i == 22))
			{
				suicide = suicide && is_suicide_move(game.get_cluster_table(),game.get_board_state(),actions[i]);
				all = all && !game.make_move(actions[i]);
				//myfile << game.make_move(actions[i]) << " for i = " << i<< std::endl;
				
			}
			else
			{
				
				suicide = suicide && !is_suicide_move(game.get_cluster_table(),game.get_board_state(),actions[i]);
				all = all && game.make_move(actions[i]);
				//myfile << game.make_move(actions[i]) << " for i = " << i<< std::endl;
				
			}
			
		}

		REQUIRE(!game.is_game_finished());
		REQUIRE(all);
		REQUIRE(suicide);
	}
	SECTION("Suicide2"){
		Game game;
		bool all = true;
		char const* filename = "sgfFiles/suicide2.sgf";
		SGFNode* treeHead = readsgffile(filename);
		std::vector<Action>actions = load_sgf_tree(treeHead);
		bool suicide = true;
		for (unsigned int i = 0; i < actions.size(); i++)
		{
			if (i == 44)
			{
				suicide = suicide && is_suicide_move(game.get_cluster_table(),game.get_board_state(),actions[i]);
				all = all && !game.make_move(actions[i]);
			}
			else
			{
				suicide = suicide && !is_suicide_move(game.get_cluster_table(),game.get_board_state(),actions[i]);
				all = all && game.make_move(actions[i]);
			}
			
		}
		REQUIRE(!game.is_game_finished());
		REQUIRE(all);
		REQUIRE(suicide);

		}
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

TEST_CASE("Testing Capture manually"){
	
	std::vector<Action> actions;
	uint32_t place = BoardState::index(1, 0);
	Action action = {place, 0};
	actions.push_back(action);
	place = BoardState::index(0, 0);
	action = {place, 1};
	actions.push_back(action);
	place = BoardState::index(0, 1);
	action = {place, 0};
	actions.push_back(action);


	place = BoardState::index(17, 0);
	action = {place, 1};
	actions.push_back(action);
	place = BoardState::index(18, 0);
	action = {place, 0};
	actions.push_back(action);

	place = BoardState::index(17, 1);
	action = {place, 1};
	actions.push_back(action);
	place = BoardState::index(18, 1);
	action = {place, 0};
	actions.push_back(action);
	place = BoardState::index(18, 2);
	action = {place, 1};
	actions.push_back(action);
	Game game ;
	bool all = true;
	
	for (unsigned int i = 0; i < actions.size(); i++)
	{
		all = all && game.make_move(actions[i]);
	}

	REQUIRE (game.get_game_state().players[0].number_captured_enemies == 1);
	REQUIRE (game.get_game_state().players[1].number_captured_enemies == 2);
	GameState game_state = game.get_game_state();
	
	
}

TEST_CASE(" Testing SGF is working Correctly"){

	std::vector<Action> manActions;
	uint32_t place = BoardState::index(18, 0);
	Action action = {place, 0};
	manActions.push_back(action);

	place = BoardState::index(18, 2);
	action = {place, 1};
	manActions.push_back(action);

	place = BoardState::index(18, 4);
	action = {place, 0};
	manActions.push_back(action);

	place = BoardState::index(15, 3);
	action = {place, 1};
	manActions.push_back(action);

	place = BoardState::index(15, 9);
	action = {place, 0};
	manActions.push_back(action);

	place = BoardState::index(9, 9);
	action = {place, 1};
	manActions.push_back(action);

	place = BoardState::index(0, 18);
	action = {place, 0};
	manActions.push_back(action);



	Game game;
	bool all = true;
	char const* filename = "sgfFiles/SGF.sgf";
	SGFNode* treeHead = readsgffile(filename);
	std::vector<Action> actions = load_sgf_tree(treeHead);

	
	for (unsigned int i = 0; i < actions.size(); i++)
	{
		
		REQUIRE (actions[i].PASS== manActions[i].PASS);
		REQUIRE (actions[i].pos == manActions[i].pos);
		REQUIRE (actions[i].player_index== manActions[i].player_index);
	}
}


