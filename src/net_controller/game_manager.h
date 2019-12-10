#ifndef GAME_ENGINE_H
#define GAME_ENGINE_H

#define ASIO_STANDALONE
#include <websocketpp/config/asio_no_tls_client.hpp>
#include <websocketpp/client.hpp>
#include <websocketpp/common/thread.hpp>
#define RAPIDJSON_HAS_STDSTRING 1
#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/prettywriter.h>
#include <iostream>
#include "controller/game.h"

using rapidjson::Document;
using rapidjson::Writer;
using rapidjson::StringBuffer;
using rapidjson::PrettyWriter;
using std::cout;
using std::string;


class GameManager {
    public:
        typedef websocketpp::client<websocketpp::config::asio_client> client;
        typedef websocketpp::config::asio_client::message_type::ptr message_ptr;

        GameManager(const std::string& uri);

        void on_open();
        void on_message(client* c, websocketpp::connection_hdl hdl, message_ptr msg);
        void on_fail(client* c, const string & uri);
        void on_close(client* c, const string & uri);
        
        void send_value(Document& value);
        void send_name();

        void pretty_print(Document  & s);

        void run();

    private:
        static constexpr int reconnection_wait_time = 4;
        const string server_address;
        client end_point;
        websocketpp::connection_hdl connection_handle;
        Document received_document;
        string player_name = "GoSlayer";
};


#endif // GAME_ENGINE_H
