#include "client.hpp"
#include <chrono>

#ifdef ROGUELIKE_SOCKET_UNIX
    // For getting error messages from system calls (errno)
    #include <cerrno> // errno
    #include <cstring> // strerror
    
    // Sockets
    #include <sys/types.h> // AF_INET (Required for BSD systems only)
    #include <sys/socket.h> // socket, bind
    #include <unistd.h> // close
    #include <netdb.h> // getaddrinfo
    #include <poll.h> // poll, pollfd
#endif

Client::Client(std::string host, unsigned short port, int timeout_ms) {
    #ifdef ROGUELIKE_SOCKET_UNIX
    
    // Create socket
    client_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(client_socket == -1)
        throw SocketException(std::strerror(errno));
    
    // Resolve host
    addrinfo* resolved_addresses;
    addrinfo hints = {
        .ai_flags = 0,
        .ai_family = AF_INET,
        .ai_socktype = SOCK_STREAM,
        .ai_protocol = 0,
        .ai_addrlen = 0,
        .ai_addr = nullptr,
        .ai_canonname = nullptr,
        .ai_next = nullptr
    };
    
    int gai_errno = getaddrinfo(host.c_str(), nullptr, &hints, &resolved_addresses);
    if(gai_errno != 0) {
        if(gai_errno == EAI_SYSTEM)
            throw SocketException(std::strerror(errno));
        else
            throw SocketException(gai_strerror(gai_errno));
    }
    
    // Create list of sockets to poll for accepted connections
    std::vector<pollfd> poll_socks;
    for(auto res = resolved_addresses; res != nullptr; res = res->ai_next) {
        // Create socket
        int candidate_sock = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
        if(candidate_sock == -1)
            continue;
        
        // Connect
        const size_t sock_addr_size = sizeof(sockaddr_in);
        sockaddr_in sock_addr = {
            .sin_family = AF_INET,
            .sin_port = htons(port),
            .sin_addr = ((sockaddr_in*)res->ai_addr)->sin_addr
        };
        
        if(connect(candidate_sock, (sockaddr*)&sock_addr, sock_addr_size) != 0) {
            close(candidate_sock);
            continue;
        }
        
        // Add to list of sockets
        poll_socks.push_back({
            .fd = candidate_sock,
            .events = POLLOUT
        });
    }
    
    // Free resolved address info
    freeaddrinfo(resolved_addresses);
    
    if(poll_socks.empty())
        throw SocketException("Failed to create socket for any resolved address");
    
    // Wait for any connection to be accepted
    int event_count = poll(poll_socks.data(), poll_socks.size(), timeout_ms);
    
    if(event_count < 1) {
        for(auto it = poll_socks.begin(); it != poll_socks.end(); it++)
            close(it->fd);
        
        if(event_count == 0)
            throw SocketException("Failed to connect to server: timed out");
        else
            throw SocketException(std::strerror(errno));
    }
    
    // Parse messages, accepting first available connection
    client_socket = -1;
    for(auto it = poll_socks.begin(); it != poll_socks.end(); it++) {
        // Pick first available connection, close all other sockets
        if(client_socket == -1 && (it->revents & POLLOUT))
            client_socket = it->fd;
        else
            close(it->fd);
    }
    
    // This won't normally happen, but sometimes connections disconnect after
    // events are polled
    if(client_socket == -1)
        throw SocketException("Failed to connect to server: timed out after events");
    
    #endif
}

Client::~Client() {
    #ifdef ROGUELIKE_SOCKET_UNIX
    
    // Close client socket
    if(client_socket != -1)
        close(client_socket);
    
    #endif
}

std::deque<std::shared_ptr<ClientMessage>> Client::receive(int timeout_ms) {
    std::deque<std::shared_ptr<ClientMessage>> messages;
    
    #ifdef ROGUELIKE_SOCKET_UNIX
    
    // Wait for an event in the client socket
    std::vector<pollfd> poll_socks = {{
        .fd = client_socket,
        .events = POLLIN
    }};
    
    int event_count = poll(poll_socks.data(), poll_socks.size(), timeout_ms);
    
    if(event_count == -1)
        throw SocketException(std::strerror(errno));
    
    // Parse events
    if(event_count == 0)
        return messages;
    
    pollfd& poll_events = poll_socks[0];
    if(poll_events.revents & POLLNVAL) {
        // The invalid flag is set when the socket is already closed
        throw SocketException("Connection lost: server disconnected (POLLNVAL)");
    }
    else if(poll_events.revents & POLLIN) {
        // If the read flag is set in the received events there is
        // available data to read.
        
        // Add data to player's read buffer
        std::array<uint8_t, 1024> read_buf;
        ssize_t bytes_read = read(poll_events.fd, read_buf.begin(), read_buf.size());
        
        // In blocking mode, a 0-byte read means the server closed
        // their socket, so, close this socket
        if(bytes_read == 0) {
            if(close(client_socket) != 0)
                throw SocketException(std::strerror(errno));
            
            throw SocketException("Connection lost: server disconnected (0-read)");
        }
        else if(bytes_read == -1)
            throw SocketException(std::strerror(errno));
        
        // Append read data to buffer
        std::vector<uint8_t> chunk(read_buf.begin(), std::next(read_buf.begin(), bytes_read));
        r_buffer.insert(chunk); // TODO allow array with given size as input for buffer
        
        // Check if a message can be built from the current read buffer.
        // Try to build as many messages as possible
        while(true) {
            std::unique_ptr<ClientMessage> message = ClientMessage::from_buffer(r_buffer);
            
            if(!message)
                break;
            // std::move used to transfer ownership to vector
            messages.push_back(std::move(message));
        }
    }
    else if(poll_events.revents & (POLLHUP | POLLERR)) {
        // If the hangup flag is set in the received events the server
        // disconnected with a TCP FIN packet. If the error flag is set
        // then the server disconnected with a TCP RST packet
        if(close(client_socket) != 0)
            throw SocketException(std::strerror(errno));
        
        throw SocketException("Connection lost: server disconnected (POLLHUP | POLLERR)");
    }
    
    #endif
    
    return messages;
}

void Client::add_message(const ClientMessage& message) {
    w_buffer.insert(message.to_bytes());
}

bool Client::send_messages(int timeout_ms) {
    if(w_buffer.size() == 0)
        return true;
    
    // Merge buffer
    std::vector<uint8_t> bytes;
    w_buffer.get(bytes, w_buffer.size());
    size_t sent = 0;
    
    // Setup timer
    std::chrono::time_point<std::chrono::high_resolution_clock> start;
    if(timeout_ms > 0)
        start = std::chrono::high_resolution_clock::now();
    
    #ifdef ROGUELIKE_SOCKET_UNIX
    
    // Start sending
    int flags = timeout_ms == -1 ? 0 : MSG_DONTWAIT;
    while(sent != bytes.size()) {
        // Send data
        ssize_t result = send(client_socket, bytes.data() + sent, bytes.size() - sent, flags);
        
        // Ignore EWOULDBLOCK and EAGAIN
        if(result < 0) {
            if(errno != EWOULDBLOCK && errno != EAGAIN)
                throw SocketException(std::strerror(errno));
        }
        
        // Clear sent part of buffer
        w_buffer.erase(result);
        sent += result;
        
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
    
    return sent == bytes.size();
}
