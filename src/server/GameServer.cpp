#include "GameServer.hpp"

void GameServer::logic() {
    int killCounter = 0;
    while(running) {
        // Get messages
        std::deque<std::shared_ptr<ServerMessage> > messages = receive(250);
        
        // Kill server if there are no players connected for 10 loops, waiting
        // on every iteration for 250ms
        if(players.empty()) {
            if(++killCounter >= 10) {
                running = false;
                break;
            }
            
            std::this_thread::sleep_for(std::chrono::milliseconds(250));
        }
        else
            killCounter = 0;
        
        // Parse messages
        for(auto it = messages.begin(); it != messages.end(); it++) {
            bool propagate = false;
            bool propagateAll = false;
            switch((*it)->type) {
                case GameMessageType::DoJoin:
                    {
                        auto joinEvent = std::dynamic_pointer_cast<ServerMessageDoJoin>(*it);
                        // Players with no name haven't joined yet
                        if(joinEvent->sender->name.empty() && !joinEvent->name.empty()) {
                            joinEvent->sender->name = joinEvent->name;
                            propagate = true;
                            propagateAll = true;
                        }
                    }
                    break;
                // TODO parse message types
            }
            
            if(propagate) {
                auto clientMessage = (*it)->toClient();
                if(!clientMessage)
                    continue;
                
                if(propagateAll)
                    addMessageAll(*clientMessage);
                else
                    addMessageAllExcept(*clientMessage, (*it)->sender);
            }
        }
        
        // Send buffered messages
        sendMessages(250);
        
        // Kill server if the socket is closed
        if(!isSocketOpen()) {
            running = false;
            return;
        }
    }
    
    if(isSocketOpen())
        close();
}

GameServer::GameServer(uint16_t port) :
    Server(port),
    running(false)
{}

GameServer::~GameServer() {
    stop();
}

void GameServer::start() {
    if(running)
        throw std::runtime_error("GameServer::start: Attempt to start server when it is already running");
    
    thread = std::thread(&GameServer::logic, this);
    running = true;
}

void GameServer::stop() {
    if(running)
        running = false;
    
    if(thread.joinable())
        thread.join();
}
