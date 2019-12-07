#ifndef SERVER_SERVER_H_
#define SERVER_SERVER_H_

#include <websocketpp/config/asio_no_tls.hpp>

#include <websocketpp/server.hpp>

#include <websocketpp/common/thread.hpp>
#include <websocketpp/common/memory.hpp>

#include <vector>
#include <string>
#include <cstdlib>
#include <iostream>
#include <functional>


typedef websocketpp::server<websocketpp::config::asio> server;
typedef websocketpp::connection_hdl connection_hdl;
typedef server::message_ptr message_ptr;
typedef websocketpp::lib::shared_ptr<websocketpp::lib::thread> thread;

class Server
{
    thread m_thread;
    server s;
    bool client_connected = false;
    connection_hdl client_con;
    std::vector<std::function<void(std::string)>> handlers;
    std::function<void()> on_open_callback = NULL;

    void on_open(connection_hdl hdl);
    void on_close(connection_hdl hdl);
    void on_message(connection_hdl hdl, message_ptr msg);
public:
    Server(uint32_t port=9002);
    void set_on_open_callback(std::function<void()> cb);
    void add_message_handler(std::function<void(std::string)> handler);
    bool has_client();
    bool send(std::string message);

    static Server* setup(uint32_t port=9002);
    ~Server();
};

#endif
