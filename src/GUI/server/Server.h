#ifndef SERVER_SERVER_H_
#define SERVER_SERVER_H_

#include <websocketpp/config/asio_no_tls.hpp>

#include <websocketpp/server.hpp>

#include <websocketpp/common/thread.hpp>
#include <websocketpp/common/memory.hpp>

#include "controller/game.h"

#include <vector>
#include <string>
#include <cstdlib>
#include <iostream>
#include <functional>


typedef websocketpp::server<websocketpp::config::asio> server;
typedef websocketpp::connection_hdl connection_hdl;
typedef server::message_ptr message_ptr;
typedef websocketpp::lib::shared_ptr<websocketpp::lib::thread> server_thread;

class Server
{
    static constexpr uint32_t DEFAULT_PORT = 9002;
    server_thread m_thread;
    server s;
    bool configed = false;
    bool client_connected = false;
    connection_hdl client_con;
    std::vector<std::function<void(std::string)>> handlers;
    std::function<void(std::string)> on_config_callback = NULL;

    void on_open(connection_hdl hdl);
    void on_close(connection_hdl hdl);
    void on_message(connection_hdl hdl, message_ptr msg);
    void send_board(go::Game* game, bool valid);
public:
    Server(uint32_t port=DEFAULT_PORT);
    void bind_game(go::Game& game);
    void set_on_config_callback(std::function<void(std::string)> cb);
    void add_message_handler(std::function<void(std::string)> handler);
    bool has_client();
    bool send(std::string message);
    void end_game(const go::Game& game);

    static Server* setup(char& mode1, char& mode2, uint32_t port=DEFAULT_PORT);
    ~Server();
};

#endif
