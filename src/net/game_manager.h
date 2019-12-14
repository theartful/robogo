#ifndef NET_GAME_MANAGER_H
#define NET_GAME_MANAGER_H

#define ASIO_STANDALONE
#include <websocketpp/config/asio_no_tls_client.hpp>
#include <websocketpp/client.hpp>
#include <websocketpp/common/thread.hpp>
#define RAPIDJSON_HAS_STDSTRING 1
#include "GUI/server/Server.h"
#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/prettywriter.h>
#include <iostream>
#include "controller/game.h"
#include <atomic>
#include "game_runner.h"


using rapidjson::Document;
using std::string;
using client = websocketpp::client<websocketpp::config::asio_client>;
using message_ptr = websocketpp::config::asio_client::message_type::ptr;
using namespace go::engine;

namespace go
{

namespace net
{

class GameManager {
    public:
        GameManager(const std::string& uri);
        void run();

        // Network callbacks.
        void on_open();
        void on_message(client* c, websocketpp::connection_hdl hdl, message_ptr msg);
        void on_fail(client* c, const string & uri);
        void on_close(client* c, const string & uri);
        // Network send-and-receive.
        void send_value(Document& value);
        void send_name();
        void send_move(const Action& action);

        void runner_lifetime_over();
    private:
        void start_game(Document& document);
        void pretty_print(Document& s);
        Action get_action(rapidjson::Value& move, uint32_t player);
        
        std::mutex runners_mutex;
        std::deque<std::shared_ptr<NetGameRunner>> runners;
        std::thread game_loop_thread;

        // TODO: wrap network stuff in a struct.
        static constexpr int reconnection_wait_time = 4;
        const string server_address;
        client end_point;
        websocketpp::connection_hdl connection_handle;
        Document received_document;
        string player_name = "GoSlayer";
        uint32_t local_player_index;
        Server* s;
};

}

}
#endif // NET_GAME_MANAGER_H