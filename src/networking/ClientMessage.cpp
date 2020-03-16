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
        case static_cast<int>(GameMessageType::Join):
            {
                // Body is a player name for Join messages
                std::string senderName;
                buffer.pop(senderName, dataSize);
                return std::unique_ptr<ClientMessage>(new ClientMessageJoin(senderName));
            }
            break;
        case static_cast<int>(GameMessageType::Quit):
            {
                // Body is a player name for Quit messages
                std::string senderName;
                buffer.pop(senderName, dataSize);
                return std::unique_ptr<ClientMessage>(new ClientMessageQuit(senderName));
            }
            break;
        case static_cast<int>(GameMessageType::Chat):
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
        case static_cast<int>(GameMessageType::MapTileData):
            {
                // Parse map dimensions
                if(dataSize == 0)
                    return nullptr;
                
                if(dataSize < 16)
                    break;
                
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
        case static_cast<int>(GameMessageType::MapObjectData):
            {
                // Parse object count
                if(dataSize == 0)
                    return nullptr;
                
                if(dataSize < 8)
                    break;
                
                std::vector<std::shared_ptr<Object>> objects;
                uint64_t count;
                buffer.pop(count);
                
                // Parse objects
                size_t dataLeft = dataSize - 8;
                for(auto o = 0; o < count; o++) {
                    // Abort if there isn't enough size for another object
                    if(dataLeft < 28) {
                        buffer.erase(dataLeft);
                        return nullptr;
                    }
                    
                    uint8_t character, omniByte, textColor, bgColor;
                    int64_t posX, posY;
                    uint64_t texHeight;
                    
                    // Character
                    buffer.pop(character);
                    
                    // Omni-byte (visible flag | direction | type)
                    // Will be split later
                    buffer.pop(omniByte);
                    
                    // Position
                    buffer.pop(posX);
                    buffer.pop(posY);
                    
                    // Formatting colors
                    buffer.pop(textColor);
                    buffer.pop(bgColor);
                    
                    // Texture height
                    buffer.pop(texHeight);
                    
                    // Texture plane
                    dataLeft -= 28;
                    std::vector<std::vector<TexturePoint> > texPlane;
                    texPlane.reserve(texHeight);
                    for(auto h = 0; h < texHeight; h++) {
                        // Abort if not enough buffer size for width value
                        if(dataLeft < 8) {
                            buffer.erase(dataLeft);
                            return nullptr;
                        }
                        
                        // Texture plane row width
                        uint64_t rowWidth;
                        buffer.pop(rowWidth);
                        dataLeft -= 8;
                        
                        // Abort if not enough buffer size for row content
                        if(dataLeft < 3 * rowWidth) {
                            buffer.erase(dataLeft);
                            return nullptr;
                        }
                        
                        // Row
                        texPlane.emplace_back();
                        auto& lastRow = texPlane[texPlane.size() - 1];
                        lastRow.reserve(3 * rowWidth);
                        for(auto c = 0; c < rowWidth; c++) {
                            uint8_t pCharacter, pTextColor, pBgColor;
                            
                            // Texture point
                            buffer.pop(pCharacter);
                            buffer.pop(pTextColor);
                            buffer.pop(pBgColor);
                            TexturePoint tPoint = {
                                static_cast<char>(pCharacter),
                                {
                                    static_cast<Color>(pTextColor),
                                    static_cast<Color>(pBgColor)
                                }
                            };
                            
                            lastRow.emplace_back(std::move(tPoint));
                        }
                        
                        dataLeft -= 3 * rowWidth;
                    }
                    
                    // Done, parse omni-byte
                    ObjectType type = static_cast<ObjectType>(omniByte       & 0b00001111);
                    Direction dir   = static_cast<Direction>((omniByte >> 4) & 0b00000111);
                    bool visible    = static_cast<bool>(     (omniByte >> 7) & 0b00000001);
                    
                    // Generate final object
                    objects.emplace_back(new Object(
                        static_cast<char>(character),
                        dir,
                        visible,
                        std::pair<int, int>(posX, posY),
                        {
                            static_cast<Color>(textColor),
                            static_cast<Color>(bgColor)
                        },
                        Texture(std::move(texPlane)),
                        type
                    ));
                }
                
                // Abort if there is remainder data
                if(dataLeft > 0) {
                    buffer.erase(dataLeft);
                    return nullptr;
                }
                
                return std::unique_ptr<ClientMessage>(new ClientMessageMapObjectData(objects));
            }
            break;
        case static_cast<int>(GameMessageType::PlayerData):
            {
                // Parse player count
                if(dataSize == 0)
                    return nullptr;
                
                if(dataSize < 8)
                    break;
                
                std::vector<std::string> names;
                uint64_t count;
                buffer.pop(count);
                
                // Parse names
                size_t dataLeft = dataSize - 8;
                for(auto n = 0; n < count; n++) {
                    // Get name size
                    if(dataLeft < 1) {
                        buffer.erase(dataLeft);
                        return nullptr;
                    }
                    
                    uint8_t nameSize;
                    buffer.pop(nameSize);
                    dataLeft--;
                    
                    // Get name
                    if(dataLeft < nameSize) {
                        buffer.erase(dataLeft);
                        return nullptr;
                    }
                    
                    std::string name;
                    buffer.pop(name, nameSize);
                    dataLeft -= nameSize;
                    
                    // Add to name list
                    names.push_back(std::move(name));
                }
                
                // Rest of needed size is known, abort if too little
                if(dataLeft != 24 * count) {
                    buffer.erase(dataLeft);
                    return nullptr;
                }
                
                std::vector<int> xPositions;
                std::vector<int> yPositions;
                std::vector<int> levels;
                
                // Parse positions
                for(auto p = 0; p < count; p++) {
                    int64_t posX, posY;
                    buffer.pop(posX);
                    buffer.pop(posY);
                    xPositions.emplace_back(posX);
                    yPositions.emplace_back(posY);
                }
                
                // Parse levels
                for(auto l = 0; l < count; l++) {
                    int64_t level;
                    buffer.pop(level);
                    levels.push_back(level);
                }
                
                // Create list of player snapshots
                std::vector<PlayerSnapshot> playerSnapshots;
                for(auto p = 0; p < count; p++)
                    playerSnapshots.emplace_back(names[p], xPositions[p], yPositions[p], levels[p]);
                
                return std::unique_ptr<ClientMessage>(new ClientMessagePlayerData(std::move(playerSnapshots)));
            }
            break;
        case static_cast<int>(GameMessageType::ActionAck):
            {
                // Parse accepted flag
                if(dataSize == 0)
                    return nullptr;
                
                if(dataSize != 1)
                    break;
                
                uint8_t accepted;
                buffer.pop(accepted);
                return std::unique_ptr<ClientMessage>(new ClientMessageActionAck(accepted));
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

ClientMessageMapTileData::ClientMessageMapTileData(Map& map) :
    ClientMessage(GameMessageType::MapTileData, "")
{
    auto mapSize = map.get_map_size();
    width = mapSize.first;
    height = mapSize.second;
    
    auto mapPlane = map.get_map_plane();
    for(auto itRow = mapPlane->begin(); itRow != mapPlane->end(); itRow++)
        tileData.push_back(*itRow);
}

ClientMessageMapTileData::ClientMessageMapTileData(MapPlane&& mapPlane, uint64_t width, uint64_t height) :
    ClientMessage(GameMessageType::MapTileData, ""),
    tileData(mapPlane),
    width(width),
    height(height)
{}

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
            uint8_t mixedByte = (uint8_t)tile.formating.text_color | ((uint8_t)tile.accesible << 6);
            data.push_back(mixedByte);
        }
    }
    
    // Generate full message with header
    return toBytesHelper(data);
}

const std::vector<uint8_t> ClientMessageMapObjectData::toBytes() const {
    std::vector<uint8_t> data;
    
    {
        Buffer buffer;
        
        // Insert object count into buffer
        buffer.insert(static_cast<uint64_t>(objects.size()));
        
        // Insert each object into buffer
        for(const auto object : objects) {
            // Character
            buffer.insert(static_cast<uint8_t>(object->get_char()));
            
            // Direction, type and visibility as a single byte
            // [1 bit - visible?][3 bits - direction][4 bits - type]
            // Note that both direction and type only use 2 bits each, but
            // since there is leftover bits for a full byte, more were used for
            // expanding in the future
            // NOTE Stored in a variable so that the final result is cast to
            // uint8_t, since bitwise operators implicitly cast to int, which
            // caused the whole encoding process to fail before. Basically,
            // DON'T TOUCH THIS
            uint8_t omniByte = (static_cast<uint8_t>(object->get_type())             & 0b00001111) |
                              ((static_cast<uint8_t>(object->get_direction())  << 4) & 0b01110000) |
                              ((static_cast<uint8_t>(object->get_visibility()) << 7) & 0b10000000);
            buffer.insert(omniByte);
            
            // Position
            auto position = object->get_position();
            buffer.insert(static_cast<int64_t>(position.first));
            buffer.insert(static_cast<int64_t>(position.second));
            
            // Formatting
            auto formatting = object->get_formating();
            buffer.insert(static_cast<uint8_t>(formatting.text_color));
            buffer.insert(static_cast<uint8_t>(formatting.background_color));
            
            // Texture
            auto texturePlane = object->get_texture().get_plane();
            // ... height
            buffer.insert(static_cast<uint64_t>(texturePlane.size()));
            for(const auto& row : texturePlane) {
                // ... row width
                buffer.insert(static_cast<uint64_t>(row.size()));
                for(const auto& point : row) {
                    // Texture point character
                    buffer.insert(static_cast<uint8_t>(point.character));
                    // Texture point formatting
                    buffer.insert(static_cast<uint8_t>(point.formating.text_color));
                    buffer.insert(static_cast<uint8_t>(point.formating.background_color));
                }
            }
        }
        
        buffer.get(data, buffer.size());
    }
    
    // Generate full message with header
    return toBytesHelper(data);
}

ClientMessagePlayerData::ClientMessagePlayerData(std::vector<std::shared_ptr<Player> >& players) :
    ClientMessage(GameMessageType::PlayerData, "")
{
    for(auto player : players) {
        if(!player->name.empty()) {
            const auto& position = player->get_position();
            playersSnapshots.emplace_back(player->name, position.first, position.second, player->level);
        }
    }
}

ClientMessagePlayerData::ClientMessagePlayerData(std::vector<PlayerSnapshot>&& playersSnapshots) :
    ClientMessage(GameMessageType::PlayerData, ""),
    playersSnapshots(playersSnapshots)
{}

const std::vector<uint8_t> ClientMessagePlayerData::toBytes() const {
    std::vector<uint8_t> data;
    
    {
        Buffer buffer;
        
        // Insert player count into buffer
        auto count = playersSnapshots.size();
        buffer.insert(static_cast<uint64_t>(count));
        
        // Insert player names into buffer
        for(auto n = 0; n < count; n++) {
            auto name = playersSnapshots[n].name;
            // Add name size; names are limited to 256 bytes
            buffer.insert(static_cast<uint8_t>(name.size()));
            // Add name bytes
            buffer.insert(name);
        }
        
        // Insert positions into buffer
        for(auto p = 0; p < count; p++) {
            buffer.insert(static_cast<int64_t>(playersSnapshots[p].x));
            buffer.insert(static_cast<int64_t>(playersSnapshots[p].y));
        }
        
        // Insert levels into buffer
        for(auto l = 0; l < count; l++)
            buffer.insert(static_cast<int64_t>(playersSnapshots[l].level));
        
        buffer.get(data, buffer.size());
    }
    
    // Generate full message with header
    return toBytesHelper(data);
}

const std::vector<uint8_t> ClientMessageActionAck::toBytes() const {
    return toBytesHelper({accepted});
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

const std::vector<uint8_t> ClientMessageDoAction::toBytes() const {
    return toBytesHelper(action.toBytes());
}
