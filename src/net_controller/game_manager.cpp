#include "game_manager.h"

using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;

GameManager::GameManager(const std::string& uri):server_address(uri)
{
    // clear logging channels
    end_point.clear_access_channels(websocketpp::log::alevel::all);
    end_point.clear_error_channels(websocketpp::log::alevel::all);
    end_point.init_asio();

    // set event handlers
    using websocketpp::lib::bind;
    end_point.set_message_handler(bind(&GameManager::on_message,this,&end_point,_1,_2));
    end_point.set_fail_handler(bind(&GameManager::on_fail,this,&end_point,server_address));
    end_point.set_close_handler(bind(&GameManager::on_close,this,&end_point,server_address));
    end_point.set_open_handler(bind(&GameManager::on_open,this));
}

void GameManager::on_open() 
{
    cout << "GameManager: Connection successfully opened. \n";
}

void GameManager::on_message(client* c, websocketpp::connection_hdl hdl, message_ptr msg) 
{
    cout << "GameManager: Got a message! \n";
    received_document.Parse(msg->get_payload().c_str());
    pretty_print(received_document);
    string message_type = received_document["type"].GetString();

    if (message_type == "NAME")
    {   
        cout << "GameManager: Message type is Main! \n";
        send_name();
    }
    else if (message_type == "START")
    {
        /*  1. CHECK IF THERE'S A NEED FOR GAME INITIALIZATION    
            2. START A NEW GAME THREAD WITH AN INITIAL CONFIGURATION (IF NEEDED) OR SEND EXTRA MOVES
        */
    }
    else if(message_type == "MOVE")
    {
        /*  SEND MOVE TO NETAGENT 
        */

    }
    else if(message_type == "END")
    {
        string end_reason=received_document["reason"].GetString();
        if(end_reason == "resign" || end_reason == "timeout" || end_reason == "pass")
        {
            //end game
        }
        else if(end_reason=="pause" || end_reason=="error")
        {
            //shouldn't I stop the game?
        }

    }
      /* TODO: 1. Initialize Game Loop if needed.
             2. RESET or KILL Game Loop.
             3. Send NetAgent an incoming move.
     */
  
}

void GameManager::send_name() 
{
    rapidjson::Document name_document;
    name_document.SetObject();
    name_document.AddMember("type","NAME",name_document.GetAllocator());
    name_document.AddMember("name",player_name,name_document.GetAllocator());
    send_value(name_document);
}

void GameManager::send_value(Document & value)
{
    StringBuffer buffer;
    Writer<StringBuffer> writer(buffer);
    value.Accept(writer);
    cout<<"Sending!\n";
    pretty_print(value);
    websocketpp::lib::error_code ec;
    end_point.send(connection_handle,buffer.GetString(),websocketpp::frame::opcode::text,ec);
    if (ec) {
        cout << "Echo failed because: " << ec.message() << "\n";
    }

}

//for debugging
void GameManager::pretty_print(Document  & s)
{ 
    StringBuffer buffer;
    PrettyWriter<StringBuffer> writer(buffer);
    buffer.Clear();
    writer.Reset(buffer); 
    s.Accept(writer);
    cout<<buffer.GetString()<<"\n";
}

void GameManager::on_fail(client* c, const std::string & uri) 
{
    std::cout << "Connection failed!!, trying to reconnect! \n";
    // try to reconnect very reconnection_wait_time seconds.
    sleep(reconnection_wait_time);
    websocketpp::lib::error_code ec;
    client::connection_ptr connection = c->get_connection(uri, ec);
    connection_handle = connection->get_handle();
    if (ec) 
    {
        std::cout << "Could not create connection because: " << ec.message() << ". \n";
        return;
    }
    c->connect(connection);
}

void GameManager::on_close(client* c, const std::string& uri) 
{
    std::cout << "Connection closed, will try to reconnect. \n";
    websocketpp::lib::error_code ec;
    client::connection_ptr connection = c->get_connection(uri, ec);
    connection_handle = connection->get_handle();
    c->connect(connection);
}


void GameManager::run()
{
    websocketpp::lib::error_code ec;
    client::connection_ptr connection = end_point.get_connection(server_address, ec);
    connection_handle = connection->get_handle();
    if (ec) 
    {
        std::cout << "Could not create connection because: " << ec.message() << std::endl;
        return;
    }
    end_point.connect(connection);
    end_point.run();
}
