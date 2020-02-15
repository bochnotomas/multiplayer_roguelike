#include "socket.hpp"

// Include remaining socket headers
#ifdef ROGUELIKE_UNIX
    // Sockets
    #include <unistd.h> // close
    #include <netdb.h> // getaddrinfo
    #include <fcntl.h> // fcntl, F_SETFL, O_NONBLOCK
#endif

void Socket::winsock2_init() {
    #ifndef ROGUELIKE_UNIX
    WSADATA wsaData;
    int startup_res = WSAStartup(MAKEWORD(2,2), &wsaData);
    if(startup_res != 0)
        throw SocketException::from_errno("Socket::winsock2_init: WSAStartup failed with code " + std::to_string(startup_res));
    #endif
}
    
void Socket::winsock2_cleanup() {
    #ifndef ROGUELIKE_UNIX
    if(WSACleanup() == SOCKET_ERROR)
        throw SocketException::from_errno("Socket::winsock2_cleanup: ");
    #endif
}

std::vector<IN_ADDR> Socket::resolve(std::string host) {
    // Resolve host
    std::vector<IN_ADDR> addresses_vec;
    PADDRINFOA resolved_addresses;
    
    int gai_errno = getaddrinfo(host.c_str(), nullptr, nullptr, &resolved_addresses);
    if(gai_errno != 0)
        throw SocketException::from_gai_errno("Socket::resolve: ", gai_errno);
    
    // Copy resolved addresses to vector
    for(auto res = resolved_addresses; res != nullptr; res = res->ai_next)
        addresses_vec.push_back(((sockaddr_in*)res->ai_addr)->sin_addr);
    
    // Free resolved address info
    freeaddrinfo(resolved_addresses);
    
    return addresses_vec;
}

Socket::Socket(SOCKET raw_sock) :
    raw_sock(raw_sock)
{
    // Validate socket
    if(!is_valid())
        throw SocketException("Socket::Socket: attempt to construct Socket with invalid raw socket");
}

Socket::Socket(SOCKET_ADDRESS_FAMILY address_family, int type, int protocol) {
    raw_sock = socket(address_family, type, protocol);
    if(!is_valid())
        throw SocketException::from_errno("Socket::Socket: ");
}

Socket::Socket() {
    invalidate();
}

Socket::Socket(Socket&& other) {
    // Take ownership of other's raw socket
    raw_sock = other.raw_sock;
    
    // Invalidate other socket
    other.invalidate();
}

Socket& Socket::operator=(Socket&& other) {
    // Don't assign yourself to yourself
    if(this != &other) {
        // Close this socket, ignoring exceptions
        try {
            close();
        }
        catch(SocketException e) {};
        
        // Take ownership of other's raw socket
        raw_sock = other.raw_sock;
        
        // Invalidate other socket
        other.invalidate();
    }
    
    return *this;
}


Socket::~Socket() {
    // Close socket if not already closed. Don't care about exceptions
    try {
        close();
    }
    catch(SocketException e) {};
}

bool Socket::operator==(const Socket& other) const {
    // Compare raw sockets
    return raw_sock == other.raw_sock;
}

bool Socket::read(std::vector<uint8_t>& output) {
    if(!is_valid())
        throw SocketException("Socket::read: Socket has already been invalidated");
    
    // Read up to 1 KiB of data
    static const size_t buffer_size = 1024;
    
    #ifdef ROGUELIKE_UNIX
    uint8_t read_buf[buffer_size];
    #else
    char read_buf[buffer_size];
    #endif
    
    ssize_t bytes_read = ::recv(raw_sock, read_buf, buffer_size, 0);
    
    if(bytes_read == SOCKET_ERROR) {
        // If non-blocking, one of these errors are set, but the socket is
        // still open, so, return true
        if(SOCKET_LAST_ERROR == SOCKET_EAGAIN || SOCKET_LAST_ERROR == SOCKET_EWOULDBLOCK)
            return true;
        throw SocketException::from_errno("Socket::read: ");
    }
    
    // If there was a 0-byte read, then the connection was closed, return false
    if(bytes_read == 0)
        return false;
    
    // Write data to output
    output.reserve(output.size() + bytes_read);
    output.insert(output.end(), read_buf, read_buf + bytes_read);
    
    return true;
}

size_t Socket::write(const uint8_t* data, size_t data_size) {
    if(!is_valid())
        throw SocketException("Socket::write: Socket has already been invalidated");
    
    // Abort on empty read
    if(!data_size)
        return 0;
    
    // Try to write as much as possible
    #ifdef ROGUELIKE_UNIX
    // Not using send on Unix because it requires the socket to be connected
    // and this is a generic socket wrapper, not just for TCP
    ssize_t bytes_written = ::write(raw_sock, data, data_size);
    #else
    int bytes_written = ::send(raw_sock, (const char*)data, data_size, 0);
    #endif
    
    if(bytes_written == SOCKET_ERROR) {
        // If non-blocking, one of these errors are set, but the socket is
        // still open
        int last_error = SOCKET_LAST_ERROR;
        if(last_error == SOCKET_EAGAIN || last_error == SOCKET_EWOULDBLOCK)
            return 0;
        throw SocketException::from_errno("Socket::write: ");
    }
    
    return bytes_written;
}

size_t Socket::write(const std::vector<uint8_t>& data) {
    return write(data.data(), data.size());
}

size_t Socket::write(const std::vector<uint8_t>::iterator begin, const std::vector<uint8_t>::iterator end) {
    return write(&(*begin), end - begin);
}

size_t Socket::write(const std::vector<uint8_t>::iterator begin, size_t data_size) {
    return write(&(*begin), data_size);
}

void Socket::bind(SOCKET_ADDRESS_FAMILY address_family, IN_ADDR address, uint16_t port) {
    if(!is_valid())
        throw SocketException("Socket::bind: Socket has already been invalidated");
    
    // Generate needed struct
    sockaddr_in sa = {
        .sin_family = address_family,
        .sin_port = htons(port),
        .sin_addr = address
    };
    
    // Bind
    if(::bind(raw_sock, (sockaddr*)&sa, sizeof(sa)) == SOCKET_ERROR)
        throw SocketException::from_errno("Socket::bind: ");
}

void Socket::bind(SOCKET_ADDRESS_FAMILY address_family, uint16_t port) {
    IN_ADDR any_address = {};
    any_address.s_addr = INADDR_ANY;
    
    bind(address_family, any_address, port);
}

void Socket::listen(int backlog) {
    if(!is_valid())
        throw SocketException("Socket::listen: Socket has already been invalidated");
    
    // Mark as listening for connections
    if(::listen(raw_sock, backlog) == SOCKET_ERROR)
        throw SocketException::from_errno("Socket::listen: ");
}

std::unique_ptr<Socket> Socket::accept() {
    if(!is_valid())
        throw SocketException("Socket::accept: Socket has already been invalidated");
    
    // Accept one incoming connection
    SOCKET accepted_raw_sock = ::accept(raw_sock, nullptr, nullptr);
    
    if(accepted_raw_sock == SOCKET_ERROR) {
        if(SOCKET_LAST_ERROR == SOCKET_EAGAIN || SOCKET_LAST_ERROR == SOCKET_EWOULDBLOCK)
            return nullptr;
        throw SocketException::from_errno("Socket::accept: ");
    }
    
    return std::unique_ptr<Socket>(new Socket(accepted_raw_sock));
}

bool Socket::connect(SOCKET_ADDRESS_FAMILY address_family, IN_ADDR address, uint16_t port) {
    if(!is_valid())
        throw SocketException("Socket::connect: Socket has already been invalidated");
    
    // Connect to given address
    const size_t sock_addr_size = sizeof(sockaddr_in);
    sockaddr_in sock_addr = {
        .sin_family = address_family,
        .sin_port = htons(port),
        .sin_addr = address
    };
    
    if(::connect(raw_sock, (sockaddr*)&sock_addr, sock_addr_size) == SOCKET_ERROR) {
        if(SOCKET_LAST_ERROR == SOCKET_EAGAIN || SOCKET_LAST_ERROR == SOCKET_EWOULDBLOCK)
            return false;
        throw SocketException::from_errno("Socket::connect: ");
    }
    
    return true;
}

void Socket::set_blocking(bool blocking) {
    if(!is_valid())
        throw SocketException("Socket::set_blocking: Socket has already been invalidated");
    
    // Set the socket into blocking or non-blocking mode
    #ifdef ROGUELIKE_UNIX
    if(blocking) {
        int old_flags = fcntl(raw_sock, F_GETFL);
        if(old_flags == SOCKET_ERROR)
            throw SocketException::from_errno("Socket::set_blocking: ");
        
        if(fcntl(raw_sock, F_SETFL, old_flags & ~O_NONBLOCK) == SOCKET_ERROR)
            throw SocketException::from_errno("Socket::set_blocking: ");
    }
    else {
        if(fcntl(raw_sock, F_SETFL, O_NONBLOCK) == SOCKET_ERROR)
            throw SocketException::from_errno("Socket::set_blocking: ");
    }
    #else
    unsigned long mode = !blocking;
    if(ioctlsocket(raw_sock, FIONBIO, &mode) == SOCKET_ERROR)
        throw SocketException::from_errno("Socket::set_blocking: ");
    #endif
}

bool Socket::is_valid() const {
    // Check if socket is valid
    #ifdef ROGUELIKE_UNIX
    return raw_sock >= 0; // Any negative value is invalid on Unix, not just -1
    #else
    return raw_sock != INVALID_SOCKET;
    #endif
}

void Socket::invalidate() {
    // Set socket to invalid value, without close
    raw_sock = INVALID_SOCKET;
}

void Socket::close() {
    // Close socket if socket is valid
    if(is_valid()) {
        #ifdef ROGUELIKE_UNIX
        int close_result = ::close(raw_sock);
        #else
        int close_result = ::closesocket(raw_sock);
        #endif
        
        if(close_result == SOCKET_ERROR)
            throw SocketException::from_errno("Socket::close: ");
        
        // Invalidate socket
        invalidate();
    }
}
