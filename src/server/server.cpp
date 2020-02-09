#include "server.hpp"
#include <unordered_map>
#include <chrono>

#ifdef ROGUELIKE_SOCKET_UNIX
    // For getting error messages from system calls (errno)
    #include <cerrno> // errno
    #include <cstring> // strerror
    
    // Sockets
    #include <sys/types.h> // AF_INET (Required for BSD systems only)
    #include <sys/socket.h> // socket, bind
    #include <arpa/inet.h> // sockaddr_in, sockaddr, htons, INADDR_ANY
    #include <unistd.h> // close
    #include <fcntl.h> // fcntl, F_SETFL, O_NONBLOCK
    #include <poll.h> // poll, pollfd
#endif

Server::Server(unsigned short port) {
    #ifdef ROGUELIKE_SOCKET_UNIX
    
    // Create socket
    listen_socket = socket(AF_INET, SOCK_STREAM, 0);
    if(listen_socket == -1)
        throw SocketException(std::strerror(errno));
    
    // Assign port to socket
    sockaddr_in sa = {
        .sin_family = AF_INET,
        .sin_port = htons(port),
        .sin_addr = {
            .s_addr = INADDR_ANY
        }
    };
    
    if(bind(listen_socket, (sockaddr*)&sa, sizeof(sa)) != 0) {
        close(listen_socket);
        throw SocketException(std::strerror(errno));
    }
    
    // Mark socket as a passive listening socket, using a maximum of 16 pending
    // connections
    if(listen(listen_socket, 16) != 0) {
        close(listen_socket);
        throw SocketException(std::strerror(errno));
    }
    
    // Mark listening socket as non-blocking
    fcntl(listen_socket, F_SETFL, O_NONBLOCK);
    
    #endif
}

Server::~Server() {
    #ifdef ROGUELIKE_SOCKET_UNIX
    
    // Close listening socket
    if(listen_socket != -1)
        close(listen_socket);
    
    #endif
}

std::deque<std::shared_ptr<ServerMessage>> Server::receive(int timeout_ms) {
    std::deque<std::shared_ptr<ServerMessage>> messages;
    
    #ifdef ROGUELIKE_SOCKET_UNIX
    
    // Accept connections from listening socket
    sockaddr addr;
    socklen_t addrlen = sizeof((sockaddr*)&addr);;
    int new_socket = accept(listen_socket, &addr, &addrlen);
    
    if(new_socket != -1) {
        // Explicitly make socket blocking
        int old_flags = fcntl(new_socket, F_GETFL);
        if(old_flags == -1) {
            close(new_socket);
            throw SocketException(std::strerror(errno));
        }
        
        fcntl(new_socket, F_SETFL, old_flags & ~O_NONBLOCK);
        
        players.emplace_back(new Player(new_socket));
    }
    else if(errno != EAGAIN || errno != EWOULDBLOCK) {
        // If there are no pending connections, errno is set to EAGAIN or EWOULDBLOCK
        throw SocketException(std::strerror(errno));
    }
    
    // Add players to the list of sockets to poll and map sockets to players
    std::vector<pollfd> poll_socks;
    std::unordered_map<int, std::shared_ptr<Player>> player_sockets;
    poll_socks.reserve(players.size());
    
    for(auto it = players.begin(); it != players.end(); it++) {
        int socket = (*it)->socket;
        if(socket == -1)
            continue;
        
        poll_socks.push_back({
            .fd = socket,
            .events = POLLIN
        });
        
        player_sockets[socket] = *it;
    }
    
    // Poll for events
    int event_count = poll(poll_socks.data(), poll_socks.size(), timeout_ms);
    
    if(event_count == -1)
        throw SocketException(std::strerror(errno));
    
    // Parse events for players
    if(event_count == 0)
        return messages;
    
    for(auto it = poll_socks.begin(); it != poll_socks.end(); it++) {
        std::shared_ptr<Player> this_player = player_sockets[it->fd];
        
        if(it->revents & POLLNVAL) {
            // The invalid flag is set when the socket is already closed. Don't
            // close the socket, but set it to an invalid socket instead
            this_player->socket = -1;
            if(!this_player->name.empty())
                messages.push_back(std::shared_ptr<ServerMessage>(new ServerMessageDoQuit(this_player)));
            disconnect_player(this_player);
        }
        else if(it->revents & POLLIN) {
            // If the read flag is set in the received events there is
            // available data to read.
            
            // Add data to player's read buffer
            std::array<uint8_t, 1024> read_buf;
            ssize_t bytes_read = read(it->fd, read_buf.begin(), read_buf.size());
            
            // In blocking mode, a 0-byte read means the client closed
            // their socket, so, close this socket
            if(bytes_read == 0) {
                if(!this_player->name.empty())
                    messages.push_back(std::shared_ptr<ServerMessage>(new ServerMessageDoQuit(this_player)));
                disconnect_player(this_player);
                continue;
            }
            else if(bytes_read == -1)
                throw SocketException(std::strerror(errno));
            
            // Append read data to buffer
            std::vector<uint8_t> chunk(read_buf.begin(), std::next(read_buf.begin(), bytes_read));
            Buffer& r_buffer = this_player->r_buffer; // TODO allow array with given size as input for buffer
            r_buffer.insert(chunk);
            
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
        else if(it->revents & (POLLHUP | POLLERR)) {
            // If the hangup flag is set in the received events the client
            // disconnected with a TCP FIN packet. If the error flag is set
            // then the client disconnected with a TCP RST packet
            
            // Quit if the player joined
            if(!this_player->name.empty())
                messages.push_back(std::shared_ptr<ServerMessage>(new ServerMessageDoQuit(this_player)));
            
            disconnect_player(this_player);
        }
    }
    
    #endif
    
    return messages;
}

void Server::add_message(const ClientMessage& message, std::shared_ptr<Player> player) {
    player->w_buffer.insert(message.to_bytes());
}

void Server::add_message_all_except(const ClientMessage& message, std::shared_ptr<Player> player) {
    #ifdef ROGUELIKE_SOCKET_UNIX
    
    for(auto it = players.begin(); it != players.end(); it++) {
        if((*it)->socket != player->socket)
            (*it)->w_buffer.insert(message.to_bytes());
    }
    
    #endif
}

void Server::add_message_all(const ClientMessage& message) {
    for(auto it = players.begin(); it != players.end(); it++)
        (*it)->w_buffer.insert(message.to_bytes());
}

bool Server::send_messages(int timeout_ms) {
    #ifdef ROGUELIKE_SOCKET_UNIX
    
    // Merge buffers
    std::unordered_map<int, size_t> all_sent;
    std::unordered_map<int, std::vector<uint8_t>> all_bytes;
    for(auto it = players.begin(); it != players.end(); it++) {
        if((*it)->w_buffer.size() == 0)
            continue;
        
        // Add to bytes to send
        std::vector<uint8_t> this_buffer;
        (*it)->w_buffer.get(this_buffer, (*it)->w_buffer.size());
        all_sent[(*it)->socket] = 0;
        all_bytes[(*it)->socket] = this_buffer;
    }
    
    // Abort if all buffers were empty
    if(all_bytes.empty())
        return true;
        
    // Setup timer
    std::chrono::time_point<std::chrono::high_resolution_clock> start;
    if(timeout_ms > 0)
        start = std::chrono::high_resolution_clock::now();
    
    // Poll for the ability to write and then send, for ALL buffered sockets
    bool failed = false;
    while(!all_bytes.empty()) {
        // Prepare to poll
        std::vector<pollfd> poll_socks;
        poll_socks.reserve(all_bytes.size());
        for(auto it = all_bytes.begin(); it != all_bytes.end(); it++) {
            poll_socks.push_back({
                .fd = it->first,
                .events = POLLOUT
            });
        }
        
        // Poll for possible writes
        int event_count = poll(poll_socks.data(), poll_socks.size(), timeout_ms);
        
        if(event_count == -1) {
            failed = true;
            break;
        }
        
        // Start sending
        for(auto it = poll_socks.begin(); it != poll_socks.end(); it++) {
            // Send data
            size_t& sent = all_sent[it->fd];
            std::vector<uint8_t>& bytes = all_bytes[it->fd];
            ssize_t result = send(it->fd, bytes.data() + sent, bytes.size() - sent, MSG_DONTWAIT);
            
            // Ignore EWOULDBLOCK and EAGAIN
            if(result < 0) {
                if(errno != EWOULDBLOCK && errno != EAGAIN) {
                    failed = true;
                    break;
                }
            }
            
            sent += result;
            
            // Remove from bytes list if all sent
            if(bytes.size() == sent)
                all_bytes.erase(it->fd);
        }
        
        if(failed)
            break;
        
        // Stop sending if timeout exceeded
        if(timeout_ms == -1) {
            auto end = std::chrono::high_resolution_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
            if(elapsed > timeout_ms)
                break;
        }
        else if(timeout_ms == 0)
            break;
    }
    
    // Clear sent part of buffer
    for(auto it = players.begin(); it != players.end(); it++) {
        if((*it)->w_buffer.size() == 0 && (*it)->socket != -1)
            continue;
        
        size_t sent = all_sent[(*it)->socket];
        if(sent > 0)
            (*it)->w_buffer.erase(sent);
    }
    
    // Throw if there was an error. This isn't done earlier because the data in
    // the write buffers that was sent needs to be cleared
    if(failed)
        throw SocketException(std::strerror(errno));
    
    #endif
    
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
