#ifndef ROGUELIKE_SERVER_HPP_INCLUDED
#define ROGUELIKE_SERVER_HPP_INCLUDED
#include "../networking/socket_exception.hpp"
#include "../networking/socket_platform.hpp"
#include "../networking/game_event.hpp"
#include "server_connection.hpp"
#include <unordered_map>

class Server {
    #ifdef ROGUELIKE_SOCKET_UNIX
    /// Listening socket for accepting connections
    int listen_socket = -1;
    /// Connected players' sockets
    std::unordered_map<int, ServerConnection> sockets;
    #endif
    
public:
    // TODO proper xml docstrings
    /// Create server with port number
    Server(unsigned short port);
    
    /// Cleanup. Closes all sockets
    ~Server();
    
    /// Receive events, with a timeout. Automatically accepts connections
    std::deque<std::shared_ptr<GameEvent>> receive(int timeout_ms);
    
    /// Add an action event to be sent to a player. Call send_events to send
    /// all buffered events
    void add_event(const GameEvent& game_event, const std::string player);
    
    /// Add an action event to be sent to all players except the one provided.
    /// Call send_events to send all buffered events
    void add_event_all_except(const GameEvent& game_event, const std::string player);
    
    /// Add an action event to be sent to all players. Call send_events to send
    /// all buffered events
    void add_event_all(const GameEvent& game_event);
    
    /// Attempt to send buffered action events. Returns true if all data has
    /// been sent. Stops sending even if not all data was sent if after
    /// timeout_ms milliseconds, unless timeout is negative where it tries
    /// forever
    bool send_events(int timeout_ms);
};

#endif
