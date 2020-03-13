#include "ServerMessage.hpp"

std::unique_ptr<ClientMessage> ServerMessage::toClient() {
    return nullptr;
}

std::unique_ptr<ServerMessage> ServerMessage::fromBuffer(Buffer& buffer, std::shared_ptr<Player> sender) {
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
    
    // Create ServerMessage
    switch(type) {
        case static_cast<int>(GameMessageType::DoJoin):
            {
                // Body is a player name for Join messages
                std::string name;
                buffer.pop(name, dataSize);
                return std::unique_ptr<ServerMessage>(new ServerMessageDoJoin(sender, name));
            }
            break;
        case static_cast<int>(GameMessageType::DoQuit):
            {
                // Body should be empty for DoQuit messages. Clear it if not
                if(dataSize > 0)
                    buffer.erase(dataSize);
                
                return std::unique_ptr<ServerMessage>(new ServerMessageDoQuit(sender));
            }
            break;
        case static_cast<int>(GameMessageType::DoChat):
            {
                // Body is a message name for DoChat messages
                std::string message;
                buffer.pop(message, dataSize);
                return std::unique_ptr<ServerMessage>(new ServerMessageDoChat(sender, message));
            }
            break;
        case static_cast<int>(GameMessageType::DoAction):
            {
                // Body is an action
                std::vector<uint8_t> body;
                buffer.pop(body, dataSize);
                try {
                    return std::unique_ptr<ServerMessage>(new ServerMessageDoAction(sender, Action::fromBytes(body)));
                }
                catch(std::invalid_argument e) {
                    return nullptr; // Will throw on invalid body
                }
            }
            break;
    }
    
    // Unknown message type or non-action message, clear body
    buffer.erase(dataSize);
    return nullptr;
}

std::unique_ptr<ClientMessage> ServerMessageDoJoin::toClient() {
    return std::unique_ptr<ClientMessage>(
        new ClientMessageJoin(name)
    );
}

std::unique_ptr<ClientMessage> ServerMessageDoQuit::toClient() {
    return std::unique_ptr<ClientMessage>(
        new ClientMessageQuit(sender->name)
    );
}

std::unique_ptr<ClientMessage> ServerMessageDoChat::toClient() {
    return std::unique_ptr<ClientMessage>(
        new ClientMessageChat(sender->name, message)
    );
}
