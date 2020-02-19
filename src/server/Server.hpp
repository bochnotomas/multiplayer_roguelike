#ifndef ROGUELIKE_SERVER_HPP_INCLUDED
#define ROGUELIKE_SERVER_HPP_INCLUDED
#include "../networking/ServerMessage.hpp"
#include "../networking/Socket.hpp"

class Server {
    /// Listening socket for accepting connections
    Socket listenSocket;
public:
    /// Connected players
    std::vector<std::shared_ptr<Player> > players;
    
    // TODO proper xml docstrings
    /// Create server with port number
    Server(uint16_t port);
    
    /// Receive messages, with a timeout. Automatically accepts connections. If
    /// there are no players connected, then this will immediately return
    std::deque<std::shared_ptr<ServerMessage> > receive(int timeoutMs);
    
    /// Add a message to be sent to a player. Call sendMessages to send all
    /// buffered messages
    void addMessage(const ClientMessage& message, std::shared_ptr<Player> player);
    
    /// Add a message to be sent to all players except the one provided.
    /// Call sendMessages to send all buffered message
    void addMessageAllExcept(const ClientMessage& message, std::shared_ptr<Player> player);
    
    /// Add a message to be sent to all players. Call sendMessages to send
    /// all buffered messages
    void addMessageAll(const ClientMessage& message);
    
    /// Attempt to send buffered messages. Returns true if all data has
    /// been sent. Stops sending even if not all data was sent if after
    /// timeoutMs milliseconds, unless timeout is negative where it tries
    /// forever
    bool sendMessages(int timeoutMs);
    
    /// Disconnects a player. Their socket is automatically closed
    void disconnectPlayer(std::shared_ptr<Player> player);
};

#endif
