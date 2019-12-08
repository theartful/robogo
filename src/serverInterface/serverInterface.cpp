
#include "serverInterface.hpp"


ServerInterface::ServerInterface(const string & uri):disconnected(true),new_message(false),curr_state(INIT),my_uri(uri),player_name("GoSlayer"){
    // clear logging channels
    end_point.clear_access_channels(websocketpp::log::alevel::all);
    end_point.clear_error_channels(websocketpp::log::alevel::all);
    end_point.init_asio();
    // set event handlers
    using websocketpp::lib::placeholders::_1;
    using websocketpp::lib::placeholders::_2;
    using websocketpp::lib::bind;
    end_point.set_message_handler(bind(&ServerInterface::on_message,this,&end_point,_1,_2));
    end_point.set_fail_handler(bind(&ServerInterface::on_fail,this,&end_point,my_uri));
    end_point.set_close_handler(bind(&ServerInterface::on_close,this,&end_point,my_uri));
    end_point.set_open_handler(bind(&ServerInterface::on_open,this));
}

void ServerInterface::on_open() {
    cout<<"Connection successfully opened\n";
    scoped_lock gaurd(discon_lock);
    disconnected=false;
}

void ServerInterface::on_message(client* c, websocketpp::connection_hdl hdl, message_ptr msg) {
    cout<<"Got a message!\n";
    scoped_lock gaurd(mess_lock);
    mess_jason.Parse(msg->get_payload().c_str());
    new_message=true;   
}


void ServerInterface::on_fail(client* c,const string & uri ){
    cout<<"Connection failed!!, trying to reconnect! \n";
    sleep(4); //try to reconnect every 4 seconds
    websocketpp::lib::error_code ec;
    client::connection_ptr con = c->get_connection(uri, ec);
    my_hdl=con->get_handle();
    if (ec) {
            std::cout << "could not create connection because: " << ec.message() << std::endl;
            return ;
    }
    c->connect(con);
}

void ServerInterface::on_close(client* c,const string &  uri) {
    {//disconnected for somereason
        scoped_lock gaurd(discon_lock);
        disconnected=true;
    }
    cout<<"Connection closed, will try to reconnect \n";
    websocketpp::lib::error_code ec;
    client::connection_ptr con = c->get_connection(uri, ec);
    my_hdl=con->get_handle();
    c->connect(con);
}

void ServerInterface::send_value( Document & v){
    StringBuffer buffer;
    Writer<StringBuffer> writer(buffer);
    buffer.Clear();
    writer.Reset(buffer); 
    v.Accept(writer);
    cout<<"Sending!\n";
    pretty_print(v);
    websocketpp::lib::error_code ec;
    end_point.send(my_hdl,buffer.GetString(),websocketpp::frame::opcode::text,ec);
    if (ec) {
        cout << "Echo failed because: " << ec.message() << std::endl;
    }

}

void ServerInterface::send_name(){
    Document message;
    message.SetObject();
    message.AddMember("type","NAME",message.GetAllocator());
    message.AddMember("name",player_name,message.GetAllocator());
    send_value(message);
}

STATE  ServerInterface::configure_game(){
    cout<<"ko "<<initial_config["ko"].GetBool()<<endl;
    cout<<"komiii value "<<initial_config["komi"].GetDouble()<<endl;
    pretty_print(initial_config);

    SizeType move_num=initial_config["moveLog"].Size();
    my_turn=((initial_config["initialState"]["turn"].GetString()==my_color) && (move_num%2==0)) ||((initial_config["initialState"]["turn"].GetString()!=my_color) && (move_num%2==1)) ;//and move log length
    cout<<"Configuring Game state, My color is '"<< my_color<<"' and my turn is "<< my_turn<<endl;
    
    if(my_turn)
        return THINKING;
    else
        return IDLE;

}

void ServerInterface::pretty_print( Document  & s){
    StringBuffer buffer;
    PrettyWriter<StringBuffer> writer(buffer);
    buffer.Clear();
    writer.Reset(buffer); 
    s.Accept(writer);
    cout<<buffer.GetString()<<endl;
}

void ServerInterface::get_move(){ 
    Value move_obj;
    int move,irow,icolumn;
    cout<<" Please select move numbr \n1-pass\n2-resign\n3-place\n";
    cin>>move;
    //construct curr_move 
    switch (move)
    {
        case 1: //pass
        { 
            move_obj.SetObject();
            move_obj.AddMember("type","pass",curr_move.GetAllocator());
        }
            break;
        case 2: //resign
        {
            move_obj.SetObject();
            move_obj.AddMember("type","resign",curr_move.GetAllocator());
        }
            break;
        case 3: //place
        {
            cout<<" please enter row then col\n";
            cin>>irow>>icolumn;
            Value point(kObjectType);
            point.AddMember("row",irow,curr_move.GetAllocator());
            point.AddMember("column",icolumn,curr_move.GetAllocator());
            move_obj.SetObject();
            move_obj.AddMember("type","place",curr_move.GetAllocator());
            move_obj.AddMember("point",point,curr_move.GetAllocator());
        }
            break;
        default:
            cout<<"Please enter valid numb\n";
            cin>>move;
    }
    curr_move.SetObject();
    curr_move.AddMember("type","MOVE",curr_move.GetAllocator());
    curr_move.AddMember("move",move_obj,curr_move.GetAllocator());
    cout<<"Constructed a move\n";
    pretty_print(curr_move);
}

void ServerInterface::send_move(){
    { 
        scoped_lock gaurd(discon_lock);
        scoped_lock gaurdm(mess_lock);
        if(new_message or disconnected){
            cout<<"Won't send move cause something happend\n";
            return;
        }
    }
    cout<<"Changing state to awaiting move response\n";
    curr_state=AWAITING_MOVE_RESPONSE;
    send_value(curr_move);
}


void ServerInterface::game_loop(){
    string mess_type;
    while(true){
        unique_lock lockd(discon_lock);
        if(disconnected){
            curr_state=IDLE;
            lockd.unlock(); //unlock disconnected
        }
        else{
            lockd.unlock();//unlock disconnected
            unique_lock lock(mess_lock);
            if(new_message){
                new_message=false;
                mess_type=mess_jason["type"].GetString();
                if (mess_type=="NAME"){
                    cout<<"Must send name\n";
                    lock.unlock(); //unlock newmess & mess_jason
                    send_name();
                    curr_state=READY;
                }
                else if(mess_type=="START" && curr_state==READY){// a game initiaization method
                    cout<<"Got a start message and in ready state\n";
                    my_color=mess_jason["color"].GetString();
                    initial_config.CopyFrom(mess_jason["configuration"], mess_jason.GetAllocator());
                    lock.unlock();//unlock newmess & mess_jason
                    curr_state=configure_game();
                }
                else if(mess_type=="MOVE"&&  curr_state==IDLE){ //got a move!
                    curr_move.CopyFrom(mess_jason["move"],mess_jason.GetAllocator());
                    remaining_time.CopyFrom(mess_jason["remainingTime"],mess_jason.GetAllocator());
                    lock.unlock(); //unlock newmess & mess_jason
                    cout<<"Got a move from my oponent\n";
                    pretty_print(curr_move);
                    pretty_print(remaining_time);
                    curr_state=THINKING;
                }
                else if( mess_type=="END"  ){
                    Document ending;
                    ending.CopyFrom(mess_jason,mess_jason.GetAllocator());
                    cout<<"Game just ended sadly so\n";
                    pretty_print(ending);
                    lock.unlock(); //unlock newmess & mess_jason
                    curr_state=READY;
                }
                else if(curr_state==AWAITING_MOVE_RESPONSE){
                    if(mess_type=="VALID"){
                        cout<<"Changing state to IDLE, move valid\n";
                        curr_state=IDLE;
                    }else if(mess_type=="INVALID"){
                        cout<<"Back to rethinking, move is invalid\n";
                        curr_state=THINKING;
                    }
                    lock.unlock(); //unlock newmess & mess_jason
                }
            }
            else{
                lock.unlock(); //unlock newmess & mess_jason
                if( curr_state==THINKING){
                    get_move();
                    send_move();
                
                }
                else if(curr_state==IDLE){
                    //whatever we want to do, till now nothing.
                }

            }

        }


    }
    
}



void ServerInterface::run(){
    websocketpp::lib::error_code ec;
    client::connection_ptr con = end_point.get_connection(my_uri, ec);
    my_hdl=con->get_handle();
    if (ec) {
        std::cout << "could not create connection because: " << ec.message() << std::endl;
        return ;
    }
    end_point.connect(con);
    //start endpoint connection thread
    websocketpp::lib::thread asio_thread(&client::run, &end_point);
    //could be a seperate thread,but for now, there doesn't seem to be a need
    game_loop();
    asio_thread.join();
}

    




