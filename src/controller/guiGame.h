#ifndef GUI_GAME_H_
#define GUI_GAME_H_

#include "GUI/server/Server.h"
#include "game.h"
#include <bits/stdc++.h>
#include <thread>
#include <time.h>

namespace go
{
class GuiGame : Game
{
public:
	GuiGame(char remote_player_color, std::string remote_player_name)
	{
		std::string player_1_type =
		    (remote_player_color == 'b') ? "remote" : "cmp";
		std::string player_1_name =
		    (remote_player_color == 'b') ? remote_player_name : "GoSlayer";
		std::string player_2_type =
		    (player_1_type == "remote") ? "cmp" : "remote";
		std::string player_2_name =
		    (player_1_name == "GoSlayer") ? remote_player_name : "GoSlayer";

		command = "google-chrome "
		          "\"file://$PWD/client/"
		          "game.html?player1=" +
		          player_1_type + "&player2=" + player_2_type +
		          "&player1_name=" + player_1_name +
		          "&player2_"
		          "name=" +
		          player_2_name + "\"";

		std::thread gui(std::bind(GuiGame::open_gui, this));
		char mode1 = 'a';
		char mode2 = 'a';
		server = Server::setup(*this, mode1, mode2);

		gui.join();
	}

	void open_gui()
	{
		sleep(2);
		std::system(command);
	}

private:
	Server* server;
	std::string command;
};
} // namespace go

#endif
