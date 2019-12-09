#include "gui.h"
#include <mutex>
#include <vector>
#include <stdexcept>
#include <functional>
using namespace go::gui;
using namespace go::engine;


BoardGUI::BoardGUI(Server* s, char m, std::string c, Agent* a)
{
    if (s == NULL)
        throw std::invalid_argument("invalid argment expected Server but found NULL");

    if (!(m == 'h' || m == 'H' || m == 'a' || m == 'A'))
        throw std::invalid_argument("mode must be either h for human or a for agent");

    if ((m == 'a' || m == 'A') && a == NULL)
        throw std::invalid_argument("invalid argment expected Agent but found NULL");
    
    server = s;
    to_lower(c);
    color = Color(c);
    id = (color.val() == "b") ? 1u : 2u;
    agent = a;
    mode = (m == 'h' || m == 'H') ? 'h' : 'a';

    using std::placeholders::_1;
    std::function<void(std::string)> res_handler = std::bind(&BoardGUI::response_handler, this, _1);
    server->add_message_handler(res_handler);
}

uint32_t BoardGUI::generate_move(const Game& game)
{
    if (mode == 'a')
    {
        uint32_t agent_move = agent->generate_move(game);
        uint32_t row = BOARD_SIZE - (agent_move / BOARD_SIZE);
        uint32_t column = (agent_move % BOARD_SIZE) + 1;
        Move move = Move(color, Vertex(row, column));

        std::vector<std::string> args;
        args.push_back(move.val());
        server->send(gtp::make_request("play", args, id));

        return agent_move;
    }
    else
    {
        std::vector<std::string> args;
        args.push_back(color.val());
        server->send(gtp::make_request("genmove", args, id));
        wait_response = true;

        std::mutex m;
        std::unique_lock<std::mutex> lk(m);
        lock_gen_move.wait(lk, [this]{return !this->waiting();});
        if (response == "resign")
            return 0;

        Vertex vertex = Vertex(response);
        uint32_t row = 0;
        uint32_t column = 0;
        Vertex::indices(vertex, row, column);
        
        return engine::BoardState::index(BOARD_SIZE-row, column-1);
    }
}

bool BoardGUI::waiting()
{
    return wait_response;
}

void BoardGUI::response_handler(string res)
{
    if (!wait_response)
        return;

    std::pair<uint32_t, std::string> parsed_response = gtp::parse_response(res);
    if (parsed_response.first != id)
        return;

    res = parsed_response.second;
    response = res;
    wait_response = false;
    lock_gen_move.notify_one();
}

void BoardGUI::to_lower(std::string& str)
{
	for_each(str.begin(), str.end(), [](char& c) { c = std::tolower(c); });
}
