#include "Server.h"
#include <mutex>
#include <condition_variable>


// this is to wait for GUI connection
Server* Server::setup(uint32_t port)
{
    std::condition_variable cv;
    auto unlock = [&cv]()
    {
        cv.notify_one();
    };

    Server* s = new Server(port);
    std::cout << "waiting for GUI to connect..." << std::endl;
    s->set_on_open_callback(unlock);
    std::mutex m;
    std::unique_lock<std::mutex> lk(m);
    cv.wait(lk, [s]{return s->has_client();});

    return s;
}

Server::Server(uint32_t port)
{
    this->s.clear_access_channels(websocketpp::log::alevel::all);

    // Initialize Asio
    this->s.init_asio();

    using websocketpp::lib::bind;
    using websocketpp::lib::placeholders::_1;
    using websocketpp::lib::placeholders::_2;
    
    // Register our message handler
    this->s.set_message_handler(bind(&Server::on_message, this, _1, _2));
    this->s.set_open_handler(bind(&Server::on_open, this, _1));

    this->s.listen(port);

    this->s.start_accept();
    this->m_thread.reset(new websocketpp::lib::thread(&server::run, &s));
}

void Server::set_on_open_callback(std::function<void()> cb)
{
    if (cb != NULL)
        on_open_callback = cb;
}

void Server::add_message_handler(std::function<void(std::string)> handler)
{
    this->handlers.push_back(handler);
}

void Server::on_open(connection_hdl hdl)
{
    if (!this->client_connected)
    {
        this->client_connected = true;
        this->client_con = hdl;
        if (on_open_callback != NULL)
            on_open_callback();
    }
}

void Server::on_close(connection_hdl hdl)
{
    this->client_connected = false;
    exit(-1);
}

void Server::on_message(connection_hdl hdl, message_ptr msg)
{
    for(auto const& handler: this->handlers)
        handler(msg->get_payload());
}

bool Server::has_client()
{
    return this->client_connected;
}

bool Server::send(std::string message)
{
    if (this->client_connected)
    {
        websocketpp::lib::error_code ec;
        s.send(this->client_con, message, websocketpp::frame::opcode::text, ec);
        if (ec)
            return false;
        else
            return true;
    }
    
    return false;
}

Server::~Server()
{
    if (client_connected)
        this->s.close(this->client_con, websocketpp::close::status::normal, "Server is terminating");

    this->s.stop();
}
