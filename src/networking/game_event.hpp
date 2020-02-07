#ifndef ROGUELIKE_GAME_EVENT_HPP_INCLUDED
#define ROGUELIKE_GAME_EVENT_HPP_INCLUDED
#include "buffer.hpp"
#include <memory>

enum GameEventType {
    Join = 0,
    Quit = 1,
    Chat = 2,
    DoQuit = 101,
    DoChat = 102
};

struct GameEvent {
protected:
    /// Protected constructor. This class is factory-ish
    GameEvent(GameEventType type, std::string player = "");
    
    /// Helper for to_bytes that automatically creates message from body
    const std::vector<uint8_t> to_bytes_helper(const std::vector<uint8_t>& data) const;
    
public:
    /// Virtual destructor. Must be implemented if base classes do memory
    /// management
    virtual ~GameEvent() = default;
    
    /// The event type. See GameEventType
    const GameEventType type;
    
    /// Affected player. Only needed for server-side multiplexing. Must be at
    /// most 255 of length
    std::string player;
    
    /// Converts game event to bytes (for networking). Has no body by default.
    /// Should be implemented, but not required
    virtual const std::vector<uint8_t> to_bytes() const;
    
    /// Create a game event from a buffer. If there is enough data for a full
    /// message, buffer is (partially) popped and a new GameEvent is returned,
    /// else, nullptr is returned and buffer is not popped. This is a factory
    static std::unique_ptr<GameEvent> from_buffer(Buffer& buffer);
};

struct GameEventJoin : public GameEvent {
    /// Sent by the client if the client wants to join the game with a certain player name
    /// Sent by the server if a client with a certain player name joins the game
    GameEventJoin(std::string player) :
        GameEvent(GameEventType::Join, player)
    {};
    
    ~GameEventJoin() = default;
    const std::vector<uint8_t> to_bytes() const override;
};

struct GameEventQuit : public GameEvent {
    /// Sent by the server if a client with a certain player name quits the game
    GameEventQuit(std::string player) :
        GameEvent(GameEventType::Quit, player)
    {};
    
    ~GameEventQuit() = default;
    const std::vector<uint8_t> to_bytes() const override;
};

struct GameEventChat : public GameEvent {
    /// Sent by the server if a client with a certain player name sent a message
    const std::string message;
    
    GameEventChat(std::string player, std::string message) :
        GameEvent(GameEventType::Chat, player),
        message(message)
    {};
    
    ~GameEventChat() = default;
    const std::vector<uint8_t> to_bytes() const override;
};

struct GameEventDoQuit : public GameEvent {
    /// Sent by the client if the client wants to quit the game
    GameEventDoQuit() :
        GameEvent(GameEventType::Quit)
    {};
    
    ~GameEventDoQuit() = default;
};

struct GameEventDoChat : public GameEvent {
    /// Sent by the client if the client wants to send a message
    const std::string message;
    
    GameEventDoChat(std::string message) :
        GameEvent(GameEventType::DoChat),
        message(message)
    {};
    
    ~GameEventDoChat() = default;
    const std::vector<uint8_t> to_bytes() const override;
};

#endif
