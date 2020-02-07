#ifndef ROGUELIKE_CLIENT_HPP_INCLUDED
#define ROGUELIKE_CLIENT_HPP_INCLUDED
#include "../networking/socket_exception.hpp"
#include "../networking/socket_platform.hpp"
#include "../networking/game_event.hpp"

class Client {
    Buffer r_buffer, w_buffer;
    
    #ifdef ROGUELIKE_SOCKET_UNIX
    /// Client socket connected to server
    int client_socket = -1;
    #endif
    
public:
    /// Connect client to server via host and port, with a timeout
    Client(std::string host, unsigned short port, int timeout_ms);
    
    /// Cleanup. Close client socket
    ~Client();
    
    /// Receive events, with a timeout
    std::deque<std::shared_ptr<GameEvent>> receive(int timeout_ms);
    
    /// Add an action event to be sent. Call send_events to send all buffered
    /// events
    void add_event(const GameEvent& game_event);
    
    /// Attempt to send buffered action events. Returns true if all data has
    /// been sent. Stops sending even if not all data was sent if after
    /// timeout_ms milliseconds, unless timeout is negative where it tries
    /// forever
    bool send_events(int timeout_ms);
};

#endif
