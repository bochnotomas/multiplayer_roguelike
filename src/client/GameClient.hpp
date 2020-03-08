#ifndef ROGUELIKE_GAME_CLIENT_HPP_INCLUDED
#define ROGUELIKE_GAME_CLIENT_HPP_INCLUDED
#include "Renderer.h"
#include "Client.hpp"
#include <atomic>

class GameClient : private Client {
    /// Renderer used by this client. NOT OWNED BY THE CLIENT
    Renderer* renderer;
    
    /// Is the user still playing?
    std::atomic<bool> playing;
    
    /// Network thread
    std::thread netThread;
    
    /// Network loop. Receive and send messages without spinlooping or blocking
    void netLoop();
    
    /// Render exception message to screen
    void exceptionScreen(std::string message);
    
    /// Client logic
    void logic(Renderer* renderer);
    
public:
    /// Connect to a server with hostname/address host and port number port
    GameClient(Renderer* renderer, std::string host, uint16_t port);
};

#endif
