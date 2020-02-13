#ifndef ROGUELIKE_SERVER_HPP_INCLUDED
#define ROGUELIKE_SERVER_HPP_INCLUDED
#include "../networking/server_message.hpp"
#include "../networking/socket.hpp"

class Server {
    /// Listening socket for accepting connections
    Socket listen_socket;
public:
    /// Connected players
    std::vector<std::shared_ptr<Player>> players;
    
    // TODO proper xml docstrings
    /// Create server with port number
    Server(uint16_t port);
    
    /// Receive messages, with a timeout. Automatically accepts connections. If
    /// there are no players connected, then this will immediately return
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
