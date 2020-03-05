#ifndef ROGUELIKE_GAME_SERVER_HPP_INCLUDED
#define ROGUELIKE_GAME_SERVER_HPP_INCLUDED
#include "Server.hpp"
#include <atomic>
#include <thread>

class GameServer : private Server {
    /// True when the server is running
    std::atomic<bool> running;
    
    /// Game logic goes here... Yup...
    void logic();
public:
    /// Create a new server. Still needs to be started with GameServer::start
    GameServer(uint16_t port);
    
    /// Start the server. Returns the server's thread
    std::thread start();
    
    /// Stop the server. Make sure to join the thread before returning from
    /// main
    void stop();
};

#endif
