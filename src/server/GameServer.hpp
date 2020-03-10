#ifndef ROGUELIKE_GAME_SERVER_HPP_INCLUDED
#define ROGUELIKE_GAME_SERVER_HPP_INCLUDED
#include "Server.hpp"
#include "Map.h"
#include <atomic>
#include <thread>

class GameServer : private Server {
    /// True when the server is running
    std::atomic<bool> running;
    
    // Levels in the game
    std::vector<Map> levels;
    
    // Server thread
    std::thread thread;
    
    /// Get the n-th level. Generate levels if needed
    Map& getLevel(int n);
    
    /// Game logic goes here... Yup...
    void logic();
public:
    /// Create a new server. Still needs to be started with GameServer::start
    GameServer(uint16_t port);
    
    /// Destructor. Automatically stops the server but does not wait for the
    /// thread
    ~GameServer();
    
    /// Start the server in a thread
    void start();
    
    /// Stop the server and wait for the thread to die
    void stop();
};

#endif
