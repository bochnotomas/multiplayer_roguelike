#ifndef ROGUELIKE_CLIENT_HPP_INCLUDED
#define ROGUELIKE_CLIENT_HPP_INCLUDED
#include "../networking/socket_exception.hpp"
#include "../networking/socket_platform.hpp"
#include "../networking/client_message.hpp"

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
    
    /// Receive messages, with a timeout
    std::deque<std::shared_ptr<ClientMessage>> receive(int timeout_ms);
    
    /// Add a message to be sent. Call send_messages to send all buffered
    /// messages
    void add_message(const ClientMessage& message);
    
    /// Attempt to send buffered messages. Returns true if all data has
    /// been sent. Stops sending even if not all data was sent if after
    /// timeout_ms milliseconds, unless timeout is negative where it tries
    /// forever
    bool send_messages(int timeout_ms);
};

#endif
