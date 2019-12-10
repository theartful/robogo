#ifndef GAME_ENGINE_H
#define GAME_ENGINE_H

#include <iostream>
#include <websocketpp/config/asio_no_tls_client.hpp>
#include <websocketpp/client.hpp>
#include <websocketpp/common/thread.hpp>
#define RAPIDJSON_HAS_STDSTRING 1
#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/prettywriter.h>

#include "controller/game.h"

using rapidjson::Document;
using rapidjson::Writer;
using rapidjson::StringBuffer;
using std::cout;

class GameManager {
    public:
        typedef websocketpp::client<websocketpp::config::asio_client> client;
        typedef websocketpp::config::asio_client::message_type::ptr message_ptr;

        GameManager(const std::string& uri);

        void on_open();
        void on_message(client* c, websocketpp::connection_hdl hdl, message_ptr msg);
        void on_fail(client* c, const std::string & uri);
        void on_close(client* c, const std::string & uri);
        
        void send_value(rapidjson::Document& value);
        void send_name();

        void run();

    private:
        static constexpr int reconnection_wait_time = 4;
        const std::string server_address = "ws://localhost:8080";
        client end_point;
        websocketpp::connection_hdl connection_handle;
        rapidjson::Document received_document;
        std::string player_name = "GoSlayer";
};


#endif // GAME_ENGINE_H