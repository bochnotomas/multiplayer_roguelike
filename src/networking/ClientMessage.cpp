#include "ClientMessage.hpp"

const std::vector<uint8_t> ClientMessage::toBytesHelper(const std::vector<uint8_t>& data) const {
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

const std::vector<uint8_t> ClientMessage::toBytes() const {
    static const std::vector<uint8_t> nothing;
    return toBytesHelper(nothing);
}

std::unique_ptr<ClientMessage> ClientMessage::fromBuffer(Buffer& buffer) {
    // Abort if header not received yet (type and data size)
    if(buffer.size() < 10)
        return nullptr;
    
    // Parse data size field
    uint64_t dataSize;
    buffer.get(dataSize, 2);
    
    // Abort if body (data) not received
    if(buffer.size() < dataSize + 10)
        return nullptr;
    
    // Parse type field
    uint16_t type;
    buffer.get(type);
    
    // Clear header from buffer, full message received
    buffer.erase(10);
    
    // Create ClientMessage
    switch(type) {
        case GameMessageType::Join:
            {
                // Body is a player name for Join messages
                std::string senderName;
                buffer.pop(senderName, dataSize);
                return std::unique_ptr<ClientMessage>(new ClientMessageJoin(senderName));
            }
            break;
        case GameMessageType::Quit:
            {
                // Body is a player name for Quit messages
                std::string senderName;
                buffer.pop(senderName, dataSize);
                return std::unique_ptr<ClientMessage>(new ClientMessageQuit(senderName));
            }
            break;
        case GameMessageType::Chat:
            {
                // Body is a player name length, a player name and a chat
                // message for Chat messages.
                // Parse player name length
                if(dataSize == 0)
                    return nullptr;
                
                uint8_t senderNameLength;
                buffer.pop(senderNameLength);
                
                // Prevent buffer size heartbleed-style bugs and 0-length player names
                if(senderNameLength > dataSize - 1) {
                    buffer.erase(dataSize - 1);
                    return nullptr;
                }
                
                // Parse player name
                std::string senderName;
                if(senderNameLength > 0)
                    buffer.pop(senderName, senderNameLength);
                
                // Parse message
                std::string message;
                size_t messageLength = dataSize - senderNameLength - 1;
                if(messageLength > 0)
                    buffer.pop(message, messageLength);
                
                return std::unique_ptr<ClientMessage>(new ClientMessageChat(senderName, message));
            }
            break;
    }
    
    // Unknown message type or action message, clear body
    buffer.erase(dataSize);
    return nullptr;
}

const std::vector<uint8_t> ClientMessageJoin::toBytes() const {
    return toBytesHelper(
        std::vector<uint8_t>(senderName.begin(), senderName.end())
    );
}

const std::vector<uint8_t> ClientMessageQuit::toBytes() const {
    return toBytesHelper(
        std::vector<uint8_t>(senderName.begin(), senderName.end())
    );
}

const std::vector<uint8_t> ClientMessageChat::toBytes() const {
    Buffer bodyBuffer;
    bodyBuffer.insert(static_cast<uint8_t>(senderName.size()));
    bodyBuffer.insert(senderName);
    bodyBuffer.insert(message);
    
    std::vector<uint8_t> data;
    bodyBuffer.pop(data, 1 + senderName.size() + message.size());
    
    return toBytesHelper(data);
}

const std::vector<uint8_t> ClientMessageDoJoin::toBytes() const {
    return toBytesHelper(
        std::vector<uint8_t>(senderName.begin(), senderName.end())
    );
}

const std::vector<uint8_t> ClientMessageDoChat::toBytes() const {
    return toBytesHelper(
        std::vector<uint8_t>(message.begin(), message.end())
    );
}
