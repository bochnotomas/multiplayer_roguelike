#include "game_event.hpp"

GameEvent::GameEvent(GameEventType type, std::string player) :
    type(type),
    player(player)
{
    if(player.size() > 255)
        throw std::length_error("Player names must be at most 255 characters long");
}

const std::vector<uint8_t> GameEvent::to_bytes_helper(const std::vector<uint8_t>& data) const {
    Buffer buffer;
    
    // Append type
    buffer.insert(static_cast<uint16_t>(type));
    
    // Append data length
    buffer.insert(static_cast<uint64_t>(data.size()));
    
    // Append data
    buffer.insert(data);
    
    // Get merged bytes
    std::vector<uint8_t> bytes;
    buffer.get(bytes); // Using get because there is no need to remove
    
    return bytes;
}

const std::vector<uint8_t> GameEvent::to_bytes() const {
    static const std::vector<uint8_t> nothing;
    return to_bytes_helper(nothing);
}

std::unique_ptr<GameEvent> GameEvent::from_buffer(Buffer& buffer) {
    // Abort if header not received yet (type and data size)
    if(buffer.size() < 10)
        return nullptr;
    
    // Parse data size field
    uint64_t data_size;
    if(!buffer.get(data_size, 2))
        return nullptr;
    
    // Abort if body (data) not received
    if(buffer.size() < data_size + 10)
        return nullptr;
    
    // Parse type field
    uint16_t type;
    if(!buffer.get(type))
        return nullptr;
    
    // Clear header from buffer, full message received
    buffer.erase(10);
    
    // Create GameEvent
    switch(type) {
        case GameEventType::Join:
            {
                // Body is a player name for Join events
                std::string player;
                buffer.pop(player, data_size);
                return std::unique_ptr<GameEvent>(new GameEventJoin(player));
            }
            break;
        case GameEventType::Quit:
            {
                // Body is a player name for Quit events
                std::string player;
                buffer.pop(player, data_size);
                return std::unique_ptr<GameEvent>(new GameEventQuit(player));
            }
            break;
        case GameEventType::Chat:
            {
                // Body is a player name length, a player name and a chat message for Chat events
                // Parse player name length
                uint8_t player_length;
                if(!buffer.pop(player_length)) {
                    buffer.erase(data_size - 1);
                    return nullptr;
                }
                
                // Prevent buffer size heartbleed-style bugs and 0-length player names
                if(player_length > data_size - 1) {
                    buffer.erase(data_size - 1);
                    return nullptr;
                }
                
                // Parse player name
                std::string player;
                if(player_length > 0)
                    buffer.pop(player, player_length);
                
                // Parse message
                std::string message;
                size_t message_length = data_size - player_length - 1;
                if(message_length > 0)
                    buffer.pop(message, message_length);
                
                return std::unique_ptr<GameEvent>(new GameEventChat(player, message));
            }
            break;
        case GameEventType::DoQuit:
            {
                // Body should be empty for DoQuit action events. Clear it if not
                if(data_size > 0)
                    buffer.erase(data_size);
                
                return std::unique_ptr<GameEvent>(new GameEventDoQuit());
            }
            break;
        case GameEventType::DoChat:
            {
                // Body is a message name for DoChat action events
                std::string message;
                buffer.pop(message, data_size);
                return std::unique_ptr<GameEvent>(new GameEventDoChat(message));
            }
            break;
    }
    
    // Unknown event type, clear body
    buffer.erase(data_size);
    return nullptr;
}

const std::vector<uint8_t> GameEventJoin::to_bytes() const {
    return to_bytes_helper(
        std::vector<uint8_t>(player.begin(), player.end())
    );
}

const std::vector<uint8_t> GameEventQuit::to_bytes() const {
    return to_bytes_helper(
        std::vector<uint8_t>(player.begin(), player.end())
    );
}

const std::vector<uint8_t> GameEventChat::to_bytes() const {
    Buffer body_buffer;
    body_buffer.insert(static_cast<uint8_t>(player.size()));
    body_buffer.insert(player);
    body_buffer.insert(message);
    
    std::vector<uint8_t> data;
    body_buffer.pop(data);
    
    return to_bytes_helper(data);
}

const std::vector<uint8_t> GameEventDoChat::to_bytes() const {
    return to_bytes_helper(
        std::vector<uint8_t>(message.begin(), message.end())
    );
}
