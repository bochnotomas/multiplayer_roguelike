#include "client_message.hpp"

const std::vector<uint8_t> ClientMessage::to_bytes_helper(const std::vector<uint8_t>& data) const {
    Buffer buffer;
    
    // Append type
    buffer.insert(static_cast<uint16_t>(type));
    
    // Append data length
    buffer.insert(static_cast<uint64_t>(data.size()));
    
    // Append data
    buffer.insert(data);
    
    // Get merged bytes
    std::vector<uint8_t> bytes;
    // Using get because there is no need to remove since buffer will go out of
    // scope
    buffer.get(bytes, 10 + data.size());
    
    return bytes;
}

const std::vector<uint8_t> ClientMessage::to_bytes() const {
    static const std::vector<uint8_t> nothing;
    return to_bytes_helper(nothing);
}

std::unique_ptr<ClientMessage> ClientMessage::from_buffer(Buffer& buffer) {
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
    
    // Create ClientMessage
    switch(type) {
        case GameMessageType::Join:
            {
                // Body is a player name for Join messages
                std::string sender_name;
                buffer.pop(sender_name, data_size);
                return std::unique_ptr<ClientMessage>(new ClientMessageJoin(sender_name));
            }
            break;
        case GameMessageType::Quit:
            {
                // Body is a player name for Quit messages
                std::string sender_name;
                buffer.pop(sender_name, data_size);
                return std::unique_ptr<ClientMessage>(new ClientMessageQuit(sender_name));
            }
            break;
        case GameMessageType::Chat:
            {
                // Body is a player name length, a player name and a chat
                // message for Chat messages.
                // Parse player name length
                uint8_t sender_name_length;
                if(!buffer.pop(sender_name_length)) {
                    buffer.erase(data_size - 1);
                    return nullptr;
                }
                
                // Prevent buffer size heartbleed-style bugs and 0-length player names
                if(sender_name_length > data_size - 1) {
                    buffer.erase(data_size - 1);
                    return nullptr;
                }
                
                // Parse player name
                std::string sender_name;
                if(sender_name_length > 0)
                    buffer.pop(sender_name, sender_name_length);
                
                // Parse message
                std::string message;
                size_t message_length = data_size - sender_name_length - 1;
                if(message_length > 0)
                    buffer.pop(message, message_length);
                
                return std::unique_ptr<ClientMessage>(new ClientMessageChat(sender_name, message));
            }
            break;
    }
    
    // Unknown message type or action message, clear body
    buffer.erase(data_size);
    return nullptr;
}

const std::vector<uint8_t> ClientMessageJoin::to_bytes() const {
    return to_bytes_helper(
        std::vector<uint8_t>(sender_name.begin(), sender_name.end())
    );
}

const std::vector<uint8_t> ClientMessageQuit::to_bytes() const {
    return to_bytes_helper(
        std::vector<uint8_t>(sender_name.begin(), sender_name.end())
    );
}

const std::vector<uint8_t> ClientMessageChat::to_bytes() const {
    Buffer body_buffer;
    body_buffer.insert(static_cast<uint8_t>(sender_name.size()));
    body_buffer.insert(sender_name);
    body_buffer.insert(message);
    
    std::vector<uint8_t> data;
    body_buffer.pop(data, 1 + sender_name.size() + message.size());
    
    return to_bytes_helper(data);
}

const std::vector<uint8_t> ClientMessageDoJoin::to_bytes() const {
    return to_bytes_helper(
        std::vector<uint8_t>(sender_name.begin(), sender_name.end())
    );
}

const std::vector<uint8_t> ClientMessageDoChat::to_bytes() const {
    return to_bytes_helper(
        std::vector<uint8_t>(message.begin(), message.end())
    );
}
