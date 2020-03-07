#ifndef ROGUELIKE_GAME_CLIENT_HPP_INCLUDED
#define ROGUELIKE_GAME_CLIENT_HPP_INCLUDED
#include "Renderer.h"
#include "Client.hpp"

class GameClient : private Client {
    /// Renderer used by this client. NOT OWNED BY THE CLIENT
    Renderer* renderer;
    
    /// Render exception message to screen
    void exceptionScreen(std::string message);
    
    /// Client logic
    void logic();
    
public:
    /// Connect to a server with hostname/address host and port number port
    GameClient(Renderer* renderer, std::string host, uint16_t port);
};

#endif
