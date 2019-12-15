#include "game_manager.h"
#include "../SimpleGUI/simplegui.h"
#include "../agents/mcts_agent.h"
using namespace go;
using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;
using rapidjson::Writer;
using rapidjson::StringBuffer;
using rapidjson::PrettyWriter;
using std::cout;

namespace go
{

namespace net
{

GameManager::GameManager(const std::string& uri, Server* gui_server):server_address(uri)
{
    s = gui_server;
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

Action GameManager::get_action(rapidjson::Value& move, uint32_t player)
{
    string move_type = move["move"]["type"].GetString();
    Action action;
    // TODO: Handle deltaTime.
    if (move_type == "place") 
    {
        uint row = move["move"]["point"]["row"].GetUint();
        uint column = move["move"]["point"]["column"].GetUint();
        action = { engine::BoardState::index(row, column), player };
        cout << "\tMove: " << player << " places at [" << row << ", " << column << "].\n";
    }
    else if (move_type == "pass")
    {
        action = { engine::Action::PASS, player };
        cout << "\tMove: " << player << " passes.\n";
    }
    else
    {
        // Resign or undefined state. Handle later.
    }
    return action;
}

void GameManager::start_game(Document& document)
{
    string color = document["color"].GetString();
    local_player_index = (color == "B") ? 0 : 1;
    Document configuration;
    configuration.CopyFrom(document["configuration"], document.GetAllocator());
    auto num_moves = configuration["moveLog"].Size();
    auto moves = configuration["moveLog"].GetArray();
    std::vector<engine::Action> actions;
    // Can he give an initial state AND a move log?
    uint32_t current_player = 0;
    for (auto& move : moves) { 
        actions.push_back(get_action(move, current_player));
        current_player = (current_player + 1) % 2;
    }
    auto current_runner = std::make_shared<NetGameRunner>();
    
    current_runner->bind_gui(s);
    s->start_net_game(1-local_player_index, "RemoteAgent");

    std::lock_guard<std::mutex> lock(runners_mutex);
    runners.push_back(current_runner);
    game_loop_thread = std::thread{ &NetGameRunner::run_game, current_runner.get(), std::ref(*this), 1 - local_player_index, actions };
}

void GameManager::on_message(client* c, websocketpp::connection_hdl hdl, message_ptr msg) 
{
    cout << "GameManager: Got a message! \n";
    received_document.Parse(msg->get_payload().c_str());
    pretty_print(received_document);
    string message_type = received_document["type"].GetString();
    if (message_type == "NAME")
    {   
        cout << "GameManager: NAME received. \n";
        send_name();
    }
    else if (message_type == "START")
    {
        cout << "GameManager: START received. \n";
        start_game(received_document);

        /*  1. CHECK IF THERE'S A NEED FOR GAME INITIALIZATION    
            2. START A NEW GAME THREAD WITH AN INITIAL CONFIGURATION (IF NEEDED) OR SEND EXTRA MOVES
        */
    }
    else if(message_type == "MOVE")
    {
        std::lock_guard<std::mutex> lock(runners_mutex);
        auto current_runner = runners.back();
        cout << "GameManager: MOVE received. \n";
        Action action = get_action(received_document, 1 - local_player_index);

        current_runner->set_remote_move({ false, action });
    }
    else if(message_type == "END")
    {
        std::lock_guard<std::mutex> lock(runners_mutex);
        auto current_runner = runners.back();
        string end_reason=received_document["reason"].GetString();
        cout << "GameManager: END reached. Reason: " << end_reason << "\n";
        current_runner->set_game_end(true);
        game_loop_thread.detach();
    }
}

void GameManager::send_name() 
{
    rapidjson::Document name_document;
    name_document.SetObject();
    name_document.AddMember("type", "NAME", name_document.GetAllocator());
    name_document.AddMember("name", player_name, name_document.GetAllocator());
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

void GameManager::send_move(const Action& action)
{
    Document current_move;
    rapidjson::Value move;
    if (action.pos == action.PASS)
    {
        move.SetObject();
        move.AddMember("type", "pass", current_move.GetAllocator());
    }
    else
    {
        auto i = BoardState::get_row(action.pos);
        auto j = BoardState::get_column(action.pos);
        rapidjson::Value point(rapidjson::kObjectType);
        point.AddMember("row", i, current_move.GetAllocator());
        point.AddMember("column", j, current_move.GetAllocator());
        move.SetObject();
        move.AddMember("type", "place", current_move.GetAllocator());
        move.AddMember("point", point, current_move.GetAllocator() );
    }
    current_move.SetObject();
    current_move.AddMember("type", "MOVE", current_move.GetAllocator());
    current_move.AddMember("move", move, current_move.GetAllocator());
    send_value(current_move);
    
}

//for debugging
void GameManager::pretty_print(Document  & d)
{ 
    StringBuffer buffer;
    PrettyWriter<StringBuffer> writer(buffer);
    buffer.Clear();
    writer.Reset(buffer); 
    d.Accept(writer);
    cout << buffer.GetString() << "\n";
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

void GameManager::runner_lifetime_over()
{
    std::lock_guard<std::mutex> lock(runners_mutex);
    auto dead_runner = runners.front();
    s->end_game(dead_runner->get_game());
    runners.pop_front();
}

}

}