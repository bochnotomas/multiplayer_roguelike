#ifndef ROGUELIKE_SERVER_HPP_INCLUDED
#define ROGUELIKE_SERVER_HPP_INCLUDED
#include "../networking/socket_exception.hpp"
#include "../networking/socket_platform.hpp"
#include "../networking/server_message.hpp"

class Server {
    #ifdef ROGUELIKE_SOCKET_UNIX
    /// Listening socket for accepting connections
    int listen_socket = -1;
    /// Connected players
    std::vector<std::shared_ptr<Player>> players;
    #endif
    
public:
    // TODO proper xml docstrings
    /// Create server with port number
    Server(unsigned short port);
    
    /// Cleanup. Closes listening socket
    ~Server();
    
    /// Receive messages, with a timeout. Automatically accepts connections
    std::deque<std::shared_ptr<ServerMessage>> receive(int timeout_ms);
    
    /// Add a message to be sent to a player. Call send_messages to send all
    /// buffered messages
    void add_message(const ClientMessage& message, std::shared_ptr<Player> player);
    
    /// Add a message to be sent to all players except the one provided.
    /// Call send_messages to send all buffered message
    void add_message_all_except(const ClientMessage& message, std::shared_ptr<Player> player);
    
    /// Add a message to be sent to all players. Call send_messages to send
    /// all buffered messages
    void add_message_all(const ClientMessage& message);
    
    /// Attempt to send buffered messages. Returns true if all data has
    /// been sent. Stops sending even if not all data was sent if after
    /// timeout_ms milliseconds, unless timeout is negative where it tries
    /// forever
    bool send_messages(int timeout_ms);
    
    /// Disconnects a player. Their socket is automatically closed
    void disconnect_player(std::shared_ptr<Player> player);
};

#endif
