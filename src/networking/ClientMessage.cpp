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
        case GameMessageType::MapTileData:
            {
                // Parse map dimensions
                if(dataSize == 0)
                    return nullptr;
                
                if(dataSize < 16) {
                    buffer.erase(dataSize);
                    return nullptr;
                }
                
                uint64_t width, height, tileCount;
                buffer.pop(width);
                buffer.pop(height);
                
                // Parse tile data
                tileCount = width * height;
                if(dataSize != 16 + tileCount * 3) {
                    buffer.erase(dataSize - 16);
                    return nullptr;
                }
                
                MapPlane tileData;
                tileData.reserve(height);
                for(auto y = 0; y < height; y++) {
                    std::vector<MapPoint> row;
                    row.reserve(width);
                    
                    for(auto x = 0; x < width; x++) {
                        uint8_t first, second, third;
                        buffer.pop(first);
                        buffer.pop(second);
                        buffer.pop(third);
                        
                        MapPoint point;
                        point.character = first;
                        point.accesible = third & 0b01000000;
                        point.formating.text_color = static_cast<Color>(third & 0b00111111);
                        point.formating.background_color = static_cast<Color>(second);
                        row.push_back(point);
                    }
                    
                    tileData.push_back(row);
                }
                
                return std::unique_ptr<ClientMessage>(new ClientMessageMapTileData(std::move(tileData), width, height));
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

const std::vector<uint8_t> ClientMessageMapTileData::toBytes() const {
    // Insert map dimensions into buffer
    // Buffers are expensive, so only use them to encode data that isn't a
    // bunch of bytes
    Buffer dimBuffer;
    dimBuffer.insert(width);
    dimBuffer.insert(height);
    
    std::vector<uint8_t> data;
    dimBuffer.pop(data, 16);
    
    // Insert tile data
    data.reserve(16 + 3 * width * height); // 3 bytes per tile
    for(const auto row : tileData) {
        for(const auto tile : row) {
            // Add single byte representing tile character
            data.push_back((uint8_t)tile.character);
            
            // Add single byte representing background color
            data.push_back((uint8_t)tile.formating.background_color);
            
            // Encode accessible flag and text color into a byte
            // [1 byte - empty][1 byte - accessible][6 bytes - text_color]
            uint8_t mixedByte = (uint16_t)tile.formating.text_color | ((uint16_t)tile.accesible << 6);
            data.push_back(mixedByte);
        }
    }
    
    // Generate full message with header
    return toBytesHelper(data);
}

const std::vector<uint8_t> ClientMessagePlayerData::toBytes() const {
    Buffer buffer;
    
    // Insert player count into buffer
    buffer.insert(static_cast<uint64_t>(names.size()));
    
    // Insert player names into buffer
    for(auto name : names) {
        // Add name size; names are limited to 256 bytes
        buffer.insert(static_cast<uint8_t>(name.size()));
        // Add name bytes
        buffer.insert(name);
    }
    
    // Insert positions into buffer
    for(auto position : positions) {
        buffer.insert(static_cast<int64_t>(position.first));
        buffer.insert(static_cast<int64_t>(position.second));
    }
    
    // Insert levels into buffer
    for(auto level : levels)
        buffer.insert(static_cast<int64_t>(level));
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
