#include "server.hpp"
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
    
    // Close all server connections
    for(auto it = sockets.begin(); it != sockets.end(); it++)
        close(it->first);
    
    #endif
}

std::deque<std::shared_ptr<GameEvent>> Server::receive(int timeout_ms) {
    std::deque<std::shared_ptr<GameEvent>> game_events;
    
    #ifdef ROGUELIKE_SOCKET_UNIX
    
    // Accept connections from listening socket
    sockaddr addr;
    socklen_t addrlen = sizeof((sockaddr*)&addr);;
    int new_sock = accept(listen_socket, &addr, &addrlen);
    
    if(new_sock != -1) {
        // Explicitly make socket blocking
        int old_flags = fcntl(new_sock, F_GETFL);
        if(old_flags == -1) {
            close(new_sock);
            throw SocketException(std::strerror(errno));
        }
        
        fcntl(new_sock, F_SETFL, old_flags & ~O_NONBLOCK);
        
        sockets[new_sock] = ServerConnection();
        
        char buf[256]; // TODO remove me --begin--
        inet_ntop(AF_INET, &(((struct sockaddr_in *)&addr)->sin_addr), buf, 255);
    }
    else if(errno != EAGAIN || errno != EWOULDBLOCK) {
        // If there are no pending connections, errno is set to EAGAIN or EWOULDBLOCK
        throw SocketException(std::strerror(errno));
    }
    
    // Wait for an event in one of the sockets
    std::vector<pollfd> poll_socks;
    poll_socks.reserve(sockets.size());
    
    // Add server connections
    for(auto it = sockets.begin(); it != sockets.end(); it++) {
        poll_socks.push_back({
            .fd = it->first,
            .events = POLLIN
        });
    }
    
    int event_count = poll(poll_socks.data(), poll_socks.size(), timeout_ms);
    
    if(event_count == -1)
        throw SocketException(std::strerror(errno));
    
    // Parse events for server connections
    if(event_count == 0)
        return game_events;
    
    for(auto it = poll_socks.begin(); it != poll_socks.end(); it++) {
        if(it->revents & POLLNVAL) {
            // The invalid flag is set when the socket is already closed
            sockets.erase(it->fd);
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
                sockets.erase(it->fd);
                
                if(close(it->fd) != 0)
                    throw SocketException(std::strerror(errno));
                
                continue;
            }
            else if(bytes_read == -1)
                throw SocketException(std::strerror(errno));
            
            // Append read data to buffer
            std::vector<uint8_t> chunk(read_buf.begin(), std::next(read_buf.begin(), bytes_read));
            Buffer& r_buffer = sockets[it->fd].r_buffer; // TODO allow array with given size as input for buffer
            r_buffer.insert(chunk);
            
            // Check if a message can be built from the current read buffer
            // TODO try parsing multiple game events
            std::unique_ptr<GameEvent> game_event = GameEvent::from_buffer(r_buffer);
            if(game_event) {
                // If this is a Join event, update the ServerConnection player
                // name, else, update the event's player
                if(game_event->type == GameEventType::Join) {
                    // Ignore event if already joined
                    if(!sockets[it->fd].player.empty())
                        continue;
                    
                    // Disallow empty names
                    if(game_event->player.empty())
                        continue;
                    
                    // Check if name already taken
                    auto con_it = sockets.begin();
                    for(; con_it != sockets.end(); con_it++) {
                        if(con_it->second.player == game_event->player)
                            break;
                    }
                    
                    if(con_it != sockets.end())
                        continue;
                    
                    // Update name
                    sockets[it->fd].player = game_event->player;
                }
                else
                    game_event->player = sockets[it->fd].player;
                
                // std::move used to transfer ownership to vector
                game_events.push_back(std::move(game_event));
            }
        }
        else if(it->revents & (POLLHUP | POLLERR)) {
            // If the hangup flag is set in the received events the client
            // disconnected with a TCP FIN packet. If the error flag is set
            // then the client disconnected with a TCP RST packet
            
            // Quit if the player joined
            std::string player = sockets[it->fd].player;
            if(!player.empty())
                game_events.push_back(std::shared_ptr<GameEvent>(new GameEventQuit(player)));
            
            sockets.erase(it->fd);
            
            if(close(it->fd) != 0)
                throw SocketException(std::strerror(errno));
        }
    }
    
    #endif
    
    return game_events;
}

void Server::add_event(const GameEvent& game_event, std::string player) {
    #ifdef ROGUELIKE_SOCKET_UNIX
    
    for(auto it = sockets.begin(); it != sockets.end(); it++) {
        if(it->second.player == player)
            it->second.w_buffer.insert(game_event.to_bytes());
    }
    
    #endif
}

void Server::add_event_all_except(const GameEvent& game_event, std::string player) {
    #ifdef ROGUELIKE_SOCKET_UNIX
    
    for(auto it = sockets.begin(); it != sockets.end(); it++) {
        if(it->second.player != player)
            it->second.w_buffer.insert(game_event.to_bytes());
    }
    
    #endif
}

void Server::add_event_all(const GameEvent& game_event) {
    #ifdef ROGUELIKE_SOCKET_UNIX
    
    for(auto it = sockets.begin(); it != sockets.end(); it++)
        it->second.w_buffer.insert(game_event.to_bytes());
    
    #endif
}

bool Server::send_events(int timeout_ms) {
    #ifdef ROGUELIKE_SOCKET_UNIX
    
    // Merge buffers
    std::unordered_map<int, std::pair<size_t, std::vector<uint8_t>>> all_bytes;
    for(auto it = sockets.begin(); it != sockets.end(); it++) {
        if(it->second.w_buffer.size() == 0)
            continue;
        
        // Add to bytes to send
        std::vector<uint8_t> this_buffer;
        it->second.w_buffer.get(this_buffer);
        all_bytes[it->first] = std::pair<size_t, std::vector<uint8_t>>(0, this_buffer);
    }
    
    // Abort if all buffers were empty
    if(all_bytes.empty())
        return true;
        
    // Setup timer
    std::chrono::time_point<std::chrono::high_resolution_clock> start;
    if(timeout_ms > 0)
        start = std::chrono::high_resolution_clock::now();
    
    // Poll for the ability to write and then send, for ALL buffered sockets
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
        
        if(event_count == -1)
            throw SocketException(std::strerror(errno));
        
        // Start sending
        for(auto it = poll_socks.begin(); it != poll_socks.end(); it++) {
            // Send data
            size_t& sent = all_bytes[it->fd].first;
            std::vector<uint8_t>& bytes = all_bytes[it->fd].second;
            ssize_t result = send(it->fd, bytes.data() + sent, bytes.size() - sent, MSG_DONTWAIT);
            
            // Ignore EWOULDBLOCK and EAGAIN
            if(result < 0) {
                if(errno != EWOULDBLOCK && errno != EAGAIN)
                    throw SocketException(std::strerror(errno));
            }
            
            // Clear sent part of buffer
            sockets[it->fd].w_buffer.erase(result);
            sent += result;
            
            // Remove from bytes list if all sent
            if(bytes.size() == sent)
                all_bytes.erase(it->fd);
        }
        
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
    
    #endif
    
    return all_bytes.empty();
}
