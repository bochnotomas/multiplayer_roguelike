#include "../networking/socket_select.hpp"
#include "server.hpp"
#include <unordered_map>
#include <chrono>

Server::Server(uint16_t port) :
    // Create socket
    listen_socket(AF_INET, SOCK_STREAM, 0)
{
    // Bind socket to all addresses and given port
    listen_socket.bind(AF_INET, port);
    
    // Mark socket as a passive listening socket, using a maximum of 16 pending
    // connections
    listen_socket.listen(16);
    
    // Mark listening socket as non-blocking
    listen_socket.set_blocking(false);
}

std::deque<std::shared_ptr<ServerMessage>> Server::receive(int timeout_ms) {
    // Accept connections from listening socket. New socket is blocking
    auto new_socket = listen_socket.accept();
    
    if(new_socket != nullptr) {
        // Mark new socket as non-blocking
        new_socket->set_blocking(false);
        
        // Create new player. Move ownership of socket to new player
        players.emplace_back(new Player(new_socket.get()));
    }
    
    // Select read events from players
    SocketSelector selector;
    for(auto it = players.begin(); it != players.end(); it++)
        selector.add_wait(SelectedEventType::Read, *it);
    
    auto events = selector.wait(timeout_ms);
    
    // Parse events for players
    std::deque<std::shared_ptr<ServerMessage>> messages;
    if(events.empty())
        return messages;
    
    for(auto it = events.begin(); it != events.end(); it++) {
        // We know we passed players to the selector, so this is safe
        std::shared_ptr<Player> this_player = std::dynamic_pointer_cast<Player>(it->socket);
        
        // Add data to player's read buffer
        std::vector<uint8_t> read_buf;
        if(!this_player->read(read_buf)) {
            // Disconnect player if read tells it should
            if(!this_player->name.empty())
                messages.push_back(std::shared_ptr<ServerMessage>(new ServerMessageDoQuit(this_player)));
            disconnect_player(this_player);
        }
        else if(!read_buf.empty()) {
            // Append read data to buffer
            Buffer& r_buffer = this_player->r_buffer;
            r_buffer.insert(read_buf);
            
            // Check if a message can be built from the current read buffer.
            // Try to build as many messages as possible
            while(true) {
                std::unique_ptr<ServerMessage> message = ServerMessage::from_buffer(r_buffer, this_player);
                
                if(!message)
                    break;
                
                // std::move used to transfer ownership to vector
                messages.push_back(std::move(message));
            }
        }
    }
    
    return messages;
}

void Server::add_message(const ClientMessage& message, std::shared_ptr<Player> player) {
    player->w_buffer.insert(message.to_bytes());
}

void Server::add_message_all_except(const ClientMessage& message, std::shared_ptr<Player> player) {
    for(auto it = players.begin(); it != players.end(); it++) {
        if(*it != player) // TODO is the socket comparison operator called here?
            (*it)->w_buffer.insert(message.to_bytes());
    }
}

void Server::add_message_all(const ClientMessage& message) {
    for(auto it = players.begin(); it != players.end(); it++)
        (*it)->w_buffer.insert(message.to_bytes());
}

bool Server::send_messages(int timeout_ms) {
    // Merge buffers. Map players to merged buffers and sent bytes total
    std::unordered_map<std::shared_ptr<Player>, size_t> all_sent;
    std::unordered_map<std::shared_ptr<Player>, std::vector<uint8_t>> all_bytes;
    for(auto it = players.begin(); it != players.end(); it++) {
        if((*it)->w_buffer.size() == 0)
            continue;
        
        // Add to bytes to send
        std::vector<uint8_t> this_buffer;
        (*it)->w_buffer.get(this_buffer, (*it)->w_buffer.size());
        all_sent[*it] = 0;
        all_bytes[*it] = std::move(this_buffer);
    }
    
    // Abort if all buffers were empty
    if(all_bytes.empty())
        return true;
        
    // Setup timer
    std::chrono::time_point<std::chrono::high_resolution_clock> start;
    if(timeout_ms > 0)
        start = std::chrono::high_resolution_clock::now();
    int elapsed = 0;
    
    // Poll for the ability to write and then send, for ALL buffered sockets
    while(!all_bytes.empty()) {
        // Select write events
        SocketSelector selector;
        for(auto it = all_bytes.begin(); it != all_bytes.end(); it++)
            selector.add_wait(SelectedEventType::Write, it->first);
        
        auto events = selector.wait(timeout_ms - elapsed);
        
        // Start sending
        for(auto it = events.begin(); it != events.end(); it++) {
            // We know we passed players to the selector, so this is safe
            std::shared_ptr<Player> this_player = std::dynamic_pointer_cast<Player>(it->socket);
            
            // Send data
            size_t& sent = all_sent[this_player];
            std::vector<uint8_t>& bytes = all_bytes[this_player];
            size_t bytes_written = this_player->write(bytes.begin() + sent, bytes.size() - sent);
            sent += bytes_written;
            
            // Erase written bytes
            this_player->w_buffer.erase(bytes_written);
            
            // Remove from bytes list if all sent
            if(bytes.size() == sent) {
                all_bytes.erase(this_player);
                all_sent.erase(this_player);
            }
        }
        
        // Stop sending if timeout exceeded
        if(timeout_ms == 0)
            break;
        else if(timeout_ms != -1) {
            auto end = std::chrono::high_resolution_clock::now();
            elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
            if(elapsed >= timeout_ms)
                break;
        }
    }
    
    return all_bytes.empty();
}

void Server::disconnect_player(std::shared_ptr<Player> player) {
    for(auto it = players.begin(); it != players.end(); it++) {
        if(*it == player) {
            players.erase(it);
            return;
        }
    }
}
