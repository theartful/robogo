#include "agent.h"
#include <mutex>
#include <vector>
#include <stdexcept>
#include <functional>
using namespace go::gui;
using namespace go::engine;


HumanAgent::HumanAgent(Server* s, std::string c)
{
    if (s == NULL)
        throw std::invalid_argument("invalid argment expected Server but found NULL");
    
    server = s;
    to_lower(c);
    color = Color(c);
    id = (color.val() == "b") ? 1u : 2u;

    using std::placeholders::_1;
    std::function<void(std::string)> res_handler = std::bind(&HumanAgent::response_handler, this, _1);
    server->add_message_handler(res_handler);
}

uint32_t HumanAgent::generate_move(const Game& game)
{
    std::vector<std::string> args;
    args.push_back(color.val());
    server->send(gtp::make_request("genmove", args, id));
    wait_response = true;

    std::mutex m;
    std::unique_lock<std::mutex> lk(m);
    lock_gen_move.wait(lk, [this]{return !this->waiting();});
    if (response == "pass")
        return engine::Action::PASS;

    Vertex vertex = Vertex(response);
    uint32_t row = 0;
    uint32_t column = 0;
    Vertex::indices(vertex, row, column);
    
    return engine::BoardState::index(BOARD_SIZE-row, column-1);
}

bool HumanAgent::waiting()
{
    return wait_response;
}

void HumanAgent::response_handler(string res)
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

void HumanAgent::to_lower(std::string& str)
{
	for_each(str.begin(), str.end(), [](char& c) { c = std::tolower(c); });
}
