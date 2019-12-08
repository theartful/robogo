
#ifndef SERVER_INTERFACE_H 
#define SERVER_INTERFACE_H

#include <iostream>
#include <websocketpp/config/asio_no_tls_client.hpp>
#include <websocketpp/client.hpp>
#include <websocketpp/common/thread.hpp>
#define RAPIDJSON_HAS_STDSTRING 1
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/prettywriter.h"

using namespace std;
using namespace rapidjson;

enum STATE{
    INIT,
    READY,
    THINKING,
    AWAITING_MOVE_RESPONSE,
    IDLE
};

class ServerInterface{
    public:
        typedef websocketpp::client<websocketpp::config::asio_client> client;
        typedef websocketpp::lib::lock_guard<websocketpp::lib::mutex> scoped_lock;
        typedef websocketpp::lib::unique_lock<websocketpp::lib::mutex> unique_lock;
        typedef websocketpp::config::asio_client::message_type::ptr message_ptr;
    
        ServerInterface(const string & uri);
        void on_open();
        void on_message(client* c, websocketpp::connection_hdl hdl, message_ptr msg) ;
        void on_fail(client* c,const string & uri );
        void on_close(client* c,const string &  uri) ;
        void send_value( Document & v);
        void send_name();
        STATE configure_game();
        void pretty_print( Document  & s);
        void get_move();
        void send_move();
        void game_loop();
        void run();

    private:
        client end_point;
        websocketpp::lib::mutex discon_lock;
        bool disconnected;
        websocketpp::lib::mutex mess_lock;
        bool new_message;
        Document mess_jason;
        STATE curr_state;
        const string  my_uri;
        websocketpp::connection_hdl my_hdl;
        string player_name;
        string my_color;
        bool my_turn;
        Document initial_config;
        Document curr_move; //could be mine to send or oponent's
        Document remaining_time;
  

};


#endif