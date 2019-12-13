#ifndef SRC_GUI_GUI_H_
#define SRC_GUI_GUI_H_

#include "controller/agent.h"
#include "controller/game.h"

#include "gtp/gtp.h"
#include "GUI/server/Server.h"

#include <string>
#include <condition_variable>

namespace go
{

namespace gui
{

class HumanAgent : public Agent
{
public:
    HumanAgent() = delete;
	HumanAgent(Server* s, std::string c);
	virtual uint32_t generate_move(const Game& game) override;

    bool waiting();
    void response_handler(string res);

private:
    Server* server;
    Color color;
    uint32_t id;
    const uint32_t BOARD_SIZE = 19;

    bool wait_response = false;
    std::string response;
    std::condition_variable lock_gen_move;

    void to_lower(std::string& str);
};

} // namespace gui
} // namespace go

#endif // SRC_GUI_GUI_H_