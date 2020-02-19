#include "../networking/SocketSelector.hpp"
#include "Server.hpp"
#include <unordered_map>
#include <chrono>

Server::Server(uint16_t port) :
    // Create socket
    listenSocket(AF_INET, SOCK_STREAM, 0)
{
    // Bind socket to all addresses and given port
    listenSocket.bind(AF_INET, port);
    
    // Mark socket as a passive listening socket, using a maximum of 16 pending
    // connections
    listenSocket.listen(16);
    
    // Mark listening socket as non-blocking
    listenSocket.setBlocking(false);
}

std::deque<std::shared_ptr<ServerMessage> > Server::receive(int timeoutMs) {
    // Accept connections from listening socket. New socket is blocking
    auto newSocket = listenSocket.accept();
    
    if(newSocket != nullptr) {
        // Mark new socket as non-blocking
        newSocket->setBlocking(false);
        
        // Create new player. Move ownership of socket to new player
        players.emplace_back(new Player(newSocket.get()));
    }
    
    // Select read events from players
    SocketSelector selector;
    for(auto it = players.begin(); it != players.end(); it++)
        selector.addWait(SelectedEventType::Read, *it);
    
    auto events = selector.wait(timeoutMs);
    
    // Parse events for players
    std::deque<std::shared_ptr<ServerMessage> > messages;
    if(events.empty())
        return messages;
    
    for(auto it = events.begin(); it != events.end(); it++) {
        // We know we passed players to the selector, so this is safe
        std::shared_ptr<Player> thisPlayer = std::dynamic_pointer_cast<Player>(it->socket);
        
        // Add data to player's read buffer
        std::vector<uint8_t> readBuf;
        if(!thisPlayer->read(readBuf)) {
            // Disconnect player if read tells it should
            if(!thisPlayer->name.empty())
                messages.push_back(std::shared_ptr<ServerMessage>(new ServerMessageDoQuit(thisPlayer)));
            disconnectPlayer(thisPlayer);
        }
        else if(!readBuf.empty()) {
            // Append read data to buffer
            Buffer& rBuffer = thisPlayer->rBuffer;
            rBuffer.insert(readBuf);
            
            // Check if a message can be built from the current read buffer.
            // Try to build as many messages as possible
            while(true) {
                std::unique_ptr<ServerMessage> message = ServerMessage::fromBuffer(rBuffer, thisPlayer);
                
                if(!message)
                    break;
                
                // std::move used to transfer ownership to vector
                messages.push_back(std::move(message));
            }
        }
    }
    
    return messages;
}

void Server::addMessage(const ClientMessage& message, std::shared_ptr<Player> player) {
    player->wBuffer.insert(message.toBytes());
}

void Server::addMessageAllExcept(const ClientMessage& message, std::shared_ptr<Player> player) {
    for(auto it = players.begin(); it != players.end(); it++) {
        if(*it != player) // TODO is the socket comparison operator called here?
            (*it)->wBuffer.insert(message.toBytes());
    }
}

void Server::addMessageAll(const ClientMessage& message) {
    for(auto it = players.begin(); it != players.end(); it++)
        (*it)->wBuffer.insert(message.toBytes());
}

bool Server::sendMessages(int timeoutMs) {
    // Merge buffers. Map players to merged buffers and sent bytes total
    std::unordered_map<std::shared_ptr<Player>, size_t> allSent;
    std::unordered_map<std::shared_ptr<Player>, std::vector<uint8_t>> allBytes;
    for(auto it = players.begin(); it != players.end(); it++) {
        if((*it)->wBuffer.size() == 0)
            continue;
        
        // Add to bytes to send
        std::vector<uint8_t> thisBuffer;
        (*it)->wBuffer.get(thisBuffer, (*it)->wBuffer.size());
        allSent[*it] = 0;
        allBytes[*it] = std::move(thisBuffer);
    }
    
    // Abort if all buffers were empty
    if(allBytes.empty())
        return true;
        
    // Setup timer
    std::chrono::time_point<std::chrono::high_resolution_clock> start;
    if(timeoutMs > 0)
        start = std::chrono::high_resolution_clock::now();
    int elapsed = 0;
    
    // Poll for the ability to write and then send, for ALL buffered sockets
    while(!allBytes.empty()) {
        // Select write events
        SocketSelector selector;
        for(auto it = allBytes.begin(); it != allBytes.end(); it++)
            selector.addWait(SelectedEventType::Write, it->first);
        
        auto events = selector.wait(timeoutMs - elapsed);
        
        // Start sending
        for(auto it = events.begin(); it != events.end(); it++) {
            // We know we passed players to the selector, so this is safe
            std::shared_ptr<Player> thisPlayer = std::dynamic_pointer_cast<Player>(it->socket);
            
            // Send data
            size_t& sent = allSent[thisPlayer];
            std::vector<uint8_t>& bytes = allBytes[thisPlayer];
            size_t bytesWritten = thisPlayer->write(bytes.begin() + sent, bytes.size() - sent);
            sent += bytesWritten;
            
            // Erase written bytes
            thisPlayer->wBuffer.erase(bytesWritten);
            
            // Remove from bytes list if all sent
            if(bytes.size() == sent) {
                allBytes.erase(thisPlayer);
                allSent.erase(thisPlayer);
            }
        }
        
        // Stop sending if timeout exceeded
        if(timeoutMs == 0)
            break;
        else if(timeoutMs != -1) {
            auto end = std::chrono::high_resolution_clock::now();
            elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
            if(elapsed >= timeoutMs)
                break;
        }
    }
    
    return allBytes.empty();
}

void Server::disconnectPlayer(std::shared_ptr<Player> player) {
    for(auto it = players.begin(); it != players.end(); it++) {
        if(*it == player) {
            players.erase(it);
            return;
        }
    }
}
