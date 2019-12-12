#include "Server.h"
#include <mutex>
#include <vector>
#include <stdexcept>
#include <condition_variable>
#include "engine/board.h"
#include "gtp/gtp.h"

Server* Server::setup(go::Game& game, char& mode1, char& mode2, uint32_t port)
{
    std::condition_variable cv;
    auto unlock = [&cv, &mode1, &mode2](std::string payload)
    {
        std::stringstream stream(payload);
        std::vector<std::string> tokens;
        for (std::string token; stream >> token;)
            tokens.push_back(token);

        if (!(tokens.size() == 3 && tokens[0] == "game_config"))
            throw std::invalid_argument("invalid game configuration recieved");
        
        if (tokens[1] == "a" || tokens[1] == "h" || tokens[1] == "r")
            mode1 = tokens[1][0];
        else
            throw std::invalid_argument("invalid game configuration recieved");

        if (tokens[2] == "a" || tokens[2] == "h" || tokens[2] == "r")
            mode2 = tokens[2][0];
        else
            throw std::invalid_argument("invalid game configuration recieved");

        if (mode1 == 'r' && mode2 == 'r')
            throw std::invalid_argument("invalid game configuration recieved");

        cv.notify_one();
    };

    Server* s = new Server(port);
    std::cout << "waiting for GUI to connect..." << std::endl;
    s->set_on_config_callback(unlock);
    std::mutex m;
    std::unique_lock<std::mutex> lk(m);
    cv.wait(lk, [s]{return s->has_client();});

    using std::placeholders::_1;
    game.set_make_move_callback(std::bind(&Server::send_board, s, &game, _1));

    return s;
}

void Server::send_board(go::Game* game, bool valid)
{
    std::vector<std::string> args;
    if (valid)
    {
        std::string player_1_captured_stones = std::to_string(game->get_game_state().players[0].number_captured_enemies);
        std::string player_2_captured_stones = std::to_string(game->get_game_state().players[1].number_captured_enemies);

        uint32_t size = go::engine::BoardState::MAX_BOARD_SIZE * go::engine::BoardState::MAX_BOARD_SIZE;
        std::string board_str(size, '.');
        
        uint32_t i = 0;
        auto board = game->get_board_state().board;
        for(const auto& cell: board)
        {
            switch (cell)
            {
                case go::engine::Cell::EMPTY:
                    board_str[i++] = '.';
                    break;
                case go::engine::Cell::BLACK:
                    board_str[i++] = 'b';
                    break;
                case go::engine::Cell::WHITE:
                    board_str[i++] = 'w';
                    break;
                default:
                    break;
            }
        }

        std::string state = player_1_captured_stones + ' ' + player_2_captured_stones + ' ' + board_str;
        args.push_back(state);
    }
    else
        args.push_back("invalid");

    send(gtp::make_request("setboard", args, 0));
}

Server::Server(uint32_t port)
{
    this->s.clear_access_channels(websocketpp::log::alevel::all);

    // Initialize Asio
    this->s.init_asio();

    using websocketpp::lib::bind;
    using websocketpp::lib::placeholders::_1;
    using websocketpp::lib::placeholders::_2;
    
    // Register our message handler
    this->s.set_message_handler(bind(&Server::on_message, this, _1, _2));
    this->s.set_open_handler(bind(&Server::on_open, this, _1));

    this->s.listen(port);

    this->s.start_accept();
    this->m_thread.reset(new websocketpp::lib::thread(&server::run, &s));
}

void Server::set_on_config_callback(std::function<void(std::string)> cb)
{
    if (cb != NULL)
        on_config_callback = cb;
}

void Server::add_message_handler(std::function<void(std::string)> handler)
{
    this->handlers.push_back(handler);
}

void Server::on_open(connection_hdl hdl)
{
    if (!this->client_connected)
    {
        this->client_connected = true;
        this->client_con = hdl;
    }
}

void Server::on_close(connection_hdl hdl)
{
    this->client_connected = false;
    exit(-1);
}

void Server::on_message(connection_hdl hdl, message_ptr msg)
{
    if (!configed)
    {
        configed = true;
        on_config_callback(msg->get_payload());
    }

    for(auto const& handler: this->handlers)
        handler(msg->get_payload());
}

bool Server::has_client()
{
    return this->client_connected;
}

bool Server::send(std::string message)
{
    if (this->client_connected)
    {
        websocketpp::lib::error_code ec;
        s.send(this->client_con, message, websocketpp::frame::opcode::text, ec);
        if (ec)
            return false;
        else
            return true;
    }
    
    return false;
}

void Server::end_game(const go::Game& game)
{
    uint32_t player_1_score = game.get_game_state().players[0].total_score;
	uint32_t player_2_score = game.get_game_state().players[1].total_score;

    if (player_1_score > player_2_score)
        send("end b " + to_string(player_1_score));
    else
        send("end w " + to_string(player_2_score));
}

Server::~Server()
{
    if (client_connected)
        this->s.close(this->client_con, websocketpp::close::status::normal, "Server is terminating");

    this->s.stop();
}
