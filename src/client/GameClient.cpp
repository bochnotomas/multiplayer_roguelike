#include "GameClient.hpp"

void GameClient::logic() {
    
}

GameClient::GameClient(Renderer* renderer, std::string host, uint16_t port) :
    Client(host, port, 5000), // 5s timeout
    renderer(renderer)
{
    // Start the client logic
    try {
        logic();
    }
    catch(...) {
        // TODO
    }
}
