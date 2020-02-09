#ifndef ROGUELIKE_SERVER_MESSAGE_HPP_INCLUDED
#define ROGUELIKE_SERVER_MESSAGE_HPP_INCLUDED
#include "client_message.hpp"
#include "../server/player.hpp"

/// A message sent to a server, which cannot be sent but can be converted to a
/// ClientMessage
class ServerMessage {
protected:
    /// Protected constructor
    ServerMessage(GameMessageType type, std::shared_ptr<Player> sender) :
        type(type),
        sender(sender)
    {}
    
public:
    /// Virtual destructor. Must be implemented if base classes do memory
    /// management
    virtual ~ServerMessage() = default;
    
    /// The message type. See GameMessageType
    const GameMessageType type;
    
    /// The player that sent this message
    const std::shared_ptr<Player> sender;
    
    /// Create a client message from this server message. Should be implemented
    /// or it is assumed there is no counterpart and nullptr is returned
    virtual std::unique_ptr<ClientMessage> to_client();
    
    /// Create a game message from a buffer. If there is enough data for a full
    /// message, buffer is (partially) popped and a new ServerMessage is
    /// returned, else, nullptr is returned and buffer is not popped. This is
    /// a factory
    static std::unique_ptr<ServerMessage> from_buffer(Buffer& buffer, std::shared_ptr<Player> sender);
};

struct ServerMessageDoJoin : public ServerMessage {
    /// Sent by the client if the client wants to join the gamewith a given
    /// player name
    const std::string name;
    
    ServerMessageDoJoin(std::shared_ptr<Player> sender, std::string name) :
        ServerMessage(GameMessageType::DoJoin, sender),
        name(name)
    {};
    
    std::unique_ptr<ClientMessage> to_client();
    ~ServerMessageDoJoin() = default;
};

struct ServerMessageDoQuit : public ServerMessage {
    /// Sent by the client if the client wants to quit the game
    ServerMessageDoQuit(std::shared_ptr<Player> sender) :
        ServerMessage(GameMessageType::DoQuit, sender)
    {};
    
    std::unique_ptr<ClientMessage> to_client();
    ~ServerMessageDoQuit() = default;
};

struct ServerMessageDoChat : public ServerMessage {
    /// Sent by the client if the client wants to send a message
    const std::string message;
    
    ServerMessageDoChat(std::shared_ptr<Player> sender, std::string message) :
        ServerMessage(GameMessageType::DoChat, sender),
        message(message)
    {};
    
    std::unique_ptr<ClientMessage> to_client();
    ~ServerMessageDoChat() = default;
};

#endif
