#include "GameServer.hpp"

void GameServer::logic() {
    while(running) {
        // Get messages
        std::deque<std::shared_ptr<ServerMessage> > messages = receive(250);
        
        // Kill server if there are no players connected
        if(players.empty()) {
            stop();
            return;
        }
        
        // Parse messages
        for(auto it = messages.begin(); it != messages.end(); it++) {
            switch((*it)->type) {
                // TODO parse message types
            }
        }
        
        // Send buffered messages
        sendMessages(250);
    }
}

GameServer::GameServer(uint16_t port) :
    Server(port),
    running(false)
{}

std::thread GameServer::start() {
    if(running)
        throw std::runtime_error("GameServer::start: Attempt to start server when it is already running");
    
    return std::thread(&GameServer::logic, this);
}

void GameServer::stop() {
    running = false;
}
