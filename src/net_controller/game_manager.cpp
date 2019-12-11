#include "game_manager.h"
#include "../SimpleGUI/simplegui.h"
#include "../agents/mcts_agent.h"
using namespace go;
using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;

namespace go
{

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

void GameManager::run_game(uint32_t netagent_color, std::vector<Action> init_actions)
{
    Game game;
    auto net_agent = std::make_shared<go:: simplegui::BoardSimpleGUI>();
    auto mcts_agent = std::make_shared<MCTSAgent>();
    net_agent->set_player_idx(netagent_color);
    mcts_agent->set_player_idx(1 - netagent_color);
    game.register_agent(net_agent, netagent_color);
    game.register_agent(mcts_agent, 1 - netagent_color);
    game.force_moves(init_actions);
    game.main_loop();
}

void GameManager::start_game(Document& document)
{
    string color = document["color"].GetString();
    uint32_t player_index = (color == "B") ? 0 : 1;
    Document configuration;
    configuration.CopyFrom(document["configuration"], document.GetAllocator());
    auto num_moves = configuration["moveLog"].Size();
    auto moves = configuration["moveLog"].GetArray();
    std::vector<engine::Action> actions;
    // Can he give an initial state AND a move log?
    uint32_t current_player = 0;
    for (auto& move : moves) { 
        string move_type = move["move"]["type"].GetString();
        // TODO: Handle deltaTime.
        if (move_type == "place") 
        {
            uint row = move["move"]["point"]["row"].GetUint();
            uint column = move["move"]["point"]["column"].GetUint();
            engine::Action action = { engine::BoardState::index(row, column), current_player };
            actions.push_back(action);
            cout << "\tMove: Place at [" << row << ", " << column << "]!\n";
        }
        else if (move_type == "pass")
        {
            engine::Action action = { engine::Action::PASS, current_player };
            actions.push_back(action);
            cout << "\tMove: Pass.\n";
        }
        else
        {
            // Resign or undefined state. Handle later.
        }
        current_player = (current_player + 1) % 2;
    }
    // Need to actually start the game now.
    game_loop_thread = std::thread{ &GameManager::run_game, this, 1 - player_index, actions };
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
        cout << "GameManager: should MOVE the NetAgent! \n";
        /*  SEND MOVE TO NETAGENT 
        */
        
    }
    else if(message_type == "END")
    {
        string end_reason=received_document["reason"].GetString();
        cout << "GameManager: END reached. Reason: " << end_reason << "\n";
        game_loop_thread.join();    
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

//for debugging
void GameManager::pretty_print(Document  & s)
{ 
    StringBuffer buffer;
    PrettyWriter<StringBuffer> writer(buffer);
    buffer.Clear();
    writer.Reset(buffer); 
    s.Accept(writer);
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

}