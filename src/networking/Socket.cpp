#include "Socket.hpp"

// Include remaining socket headers
#ifdef ROGUELIKE_UNIX
    // Sockets
    #include <unistd.h> // close
    #include <netdb.h> // getaddrinfo
    #include <fcntl.h> // fcntl, F_SETFL, O_NONBLOCK
    #include <signal.h> // signal, SIGPIPE, SIG_IGN
#endif

void Socket::initSocketApi() {
    #ifndef ROGUELIKE_UNIX
    WSADATA wsaData;
    int startupRes = WSAStartup(MAKEWORD(2,2), &wsaData);
    if(startupRes != 0)
        throw SocketException::fromErrno("Socket::initSocketApi: WSAStartup failed with code " + std::to_string(startupRes));
    #else
    // Ignore SIGPIPE (in case something tries to write to a closed socket)
    signal(SIGPIPE, SIG_IGN);
    #endif
}
    
void Socket::cleanupSocketApi() {
    #ifndef ROGUELIKE_UNIX
    if(WSACleanup() == SOCKET_ERROR)
        throw SocketException::fromErrno("Socket::cleanupSocketApi: ");
    #endif
}

std::vector<IN_ADDR> Socket::resolve(std::string host) {
    // Resolve host
    std::vector<IN_ADDR> addressesVec;
    PADDRINFOA resolvedAddresses;
    
    int gaiErrno = getaddrinfo(host.c_str(), nullptr, nullptr, &resolvedAddresses);
    if(gaiErrno != 0)
        throw SocketException::fromGaiErrno("Socket::resolve: ", gaiErrno);
    
    // Copy resolved addresses to vector
    for(auto res = resolvedAddresses; res != nullptr; res = res->ai_next)
        addressesVec.push_back(((sockaddr_in*)res->ai_addr)->sin_addr);
    
    // Free resolved address info
    freeaddrinfo(resolvedAddresses);
    
    return addressesVec;
}

Socket::Socket(SOCKET rawSock) :
    rawSock(rawSock)
{
    // Validate socket
    if(!isValid())
        throw SocketException("Socket::Socket: attempt to construct Socket with invalid raw socket");
}

Socket::Socket(SOCKET_ADDRESS_FAMILY addressFamily, int type, int protocol) {
    rawSock = socket(addressFamily, type, protocol);
    if(!isValid())
        throw SocketException::fromErrno("Socket::Socket: ");
}

Socket::Socket() {
    invalidate();
}

Socket::Socket(Socket&& other) {
    // Take ownership of other's raw socket
    rawSock = other.rawSock;
    
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
        rawSock = other.rawSock;
        
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
    return rawSock == other.rawSock;
}

bool Socket::read(std::vector<uint8_t>& output) {
    if(!isValid())
        throw SocketException("Socket::read: Socket has already been invalidated");
    
    // Read up to 1 KiB of data
    static const size_t bufferSize = 1024;
    
    #ifdef ROGUELIKE_UNIX
    uint8_t readBuf[bufferSize];
    ssize_t bytesRead = ::recv(rawSock, readBuf, bufferSize, 0);
    #else
    char readBuf[bufferSize];
    int bytesRead = ::recv(rawSock, readBuf, bufferSize, 0);
    #endif
    
    if(bytesRead == SOCKET_ERROR) {
        // If non-blocking, one of these errors are set, but the socket is
        // still open, so, return true
        if(SOCKET_LAST_ERROR == SOCKET_EAGAIN || SOCKET_LAST_ERROR == SOCKET_EWOULDBLOCK)
            return true;
        throw SocketException::fromErrno("Socket::read: ");
    }
    
    // If there was a 0-byte read, then the connection was closed, return false
    if(bytesRead == 0)
        return false;
    
    // Write data to output
    output.reserve(output.size() + bytesRead);
    output.insert(output.end(), readBuf, readBuf + bytesRead);
    
    return true;
}

size_t Socket::write(const uint8_t* data, size_t dataSize) {
    if(!isValid())
        throw SocketException("Socket::write: Socket has already been invalidated");
    
    // Abort on empty read
    if(!dataSize)
        return 0;
    
    // Try to write as much as possible
    #ifdef ROGUELIKE_UNIX
    // Not using send on Unix because it requires the socket to be connected
    // and this is a generic socket wrapper, not just for TCP
    ssize_t bytesWritten = ::write(rawSock, data, dataSize);
    #else
    int bytesWritten = ::send(rawSock, (const char*)data, dataSize, 0);
    #endif
    
    if(bytesWritten == SOCKET_ERROR) {
        // If non-blocking, one of these errors are set, but the socket is
        // still open
        int lastError = SOCKET_LAST_ERROR;
        if(lastError == SOCKET_EAGAIN || lastError == SOCKET_EWOULDBLOCK)
            return 0;
        throw SocketException::fromErrno("Socket::write: ");
    }
    
    return bytesWritten;
}

size_t Socket::write(const std::vector<uint8_t>& data) {
    return write(data.data(), data.size());
}

size_t Socket::write(const std::vector<uint8_t>::iterator begin, const std::vector<uint8_t>::iterator end) {
    return write(&(*begin), end - begin);
}

size_t Socket::write(const std::vector<uint8_t>::iterator begin, size_t dataSize) {
    return write(&(*begin), dataSize);
}

void Socket::bind(SOCKET_ADDRESS_FAMILY addressFamily, IN_ADDR address, uint16_t port) {
    if(!isValid())
        throw SocketException("Socket::bind: Socket has already been invalidated");
    
    // Generate needed struct
    sockaddr_in sa;
    sa.sin_family = addressFamily;
    sa.sin_port = htons(port);
    sa.sin_addr = address;
    
    // Bind
    if(::bind(rawSock, (sockaddr*)&sa, sizeof(sa)) == SOCKET_ERROR)
        throw SocketException::fromErrno("Socket::bind: ");
}

void Socket::bind(SOCKET_ADDRESS_FAMILY addressFamily, uint16_t port) {
    IN_ADDR anyAddress{ INADDR_ANY };
    bind(addressFamily, anyAddress, port);
}

void Socket::listen(int backlog) {
    if(!isValid())
        throw SocketException("Socket::listen: Socket has already been invalidated");
    
    // Mark as listening for connections
    if(::listen(rawSock, backlog) == SOCKET_ERROR)
        throw SocketException::fromErrno("Socket::listen: ");
}

std::unique_ptr<Socket> Socket::accept() {
    if(!isValid())
        throw SocketException("Socket::accept: Socket has already been invalidated");
    
    // Accept one incoming connection
    SOCKET acceptedRawSock = ::accept(rawSock, nullptr, nullptr);
    
    if(acceptedRawSock == SOCKET_ERROR) {
        if(SOCKET_LAST_ERROR == SOCKET_EAGAIN || SOCKET_LAST_ERROR == SOCKET_EWOULDBLOCK)
            return nullptr;
        throw SocketException::fromErrno("Socket::accept: ");
    }
    
    return std::unique_ptr<Socket>(new Socket(acceptedRawSock));
}

bool Socket::connect(SOCKET_ADDRESS_FAMILY addressFamily, IN_ADDR address, uint16_t port) {
    if(!isValid())
        throw SocketException("Socket::connect: Socket has already been invalidated");
    
    // Connect to given address
    const size_t sockAddrSize = sizeof(sockaddr_in);
    sockaddr_in sockAddr;
    sockAddr.sin_family = addressFamily;
    sockAddr.sin_port = htons(port);
    sockAddr.sin_addr = address;
    
    if(::connect(rawSock, (sockaddr*)&sockAddr, sockAddrSize) == SOCKET_ERROR) {
        if(SOCKET_LAST_ERROR == SOCKET_EAGAIN || SOCKET_LAST_ERROR == SOCKET_EWOULDBLOCK)
            return false;
        throw SocketException::fromErrno("Socket::connect: ");
    }
    
    return true;
}

void Socket::shutdown(SocketShutdownMode mode) {
    if(!isValid())
        throw SocketException("Socket::shutdown: Socket has already been invalidated");
    
    int how;
    switch(mode) {
        #ifdef ROGUELIKE_UNIX
        case SocketShutdownMode::ShutRead:
            how = SHUT_RD;
            break;
        case SocketShutdownMode::ShutWrite:
            how = SHUT_WR;
            break;
        case SocketShutdownMode::ShutReadWrite:
            how = SHUT_RDWR;
            break;
        #else
        case SocketShutdownMode::ShutRead:
            how = SD_RECEIVE;
            break;
        case SocketShutdownMode::ShutWrite:
            how = SD_SEND;
            break;
        case SocketShutdownMode::ShutReadWrite:
            how = SD_BOTH;
            break;
        #endif
        default:
            throw SocketException("Socket::shutdown: Invalid mode value " +std::to_string(mode));
    }
    
    int shutdownResult = ::shutdown(rawSock, how);
    if(shutdownResult == SOCKET_ERROR)
        throw SocketException::fromErrno("Socket::shutdown: ");
}

void Socket::setBlocking(bool blocking) {
    if(!isValid())
        throw SocketException("Socket::setBlocking: Socket has already been invalidated");
    
    // Set the socket into blocking or non-blocking mode
    #ifdef ROGUELIKE_UNIX
    if(blocking) {
        int oldFlags = fcntl(rawSock, F_GETFL);
        if(oldFlags == SOCKET_ERROR)
            throw SocketException::fromErrno("Socket::setBlocking: ");
        
        if(fcntl(rawSock, F_SETFL, oldFlags & ~O_NONBLOCK) == SOCKET_ERROR)
            throw SocketException::fromErrno("Socket::setBlocking: ");
    }
    else {
        if(fcntl(rawSock, F_SETFL, O_NONBLOCK) == SOCKET_ERROR)
            throw SocketException::fromErrno("Socket::setBlocking: ");
    }
    #else
    unsigned long mode = !blocking;
    if(ioctlsocket(rawSock, FIONBIO, &mode) == SOCKET_ERROR)
        throw SocketException::fromErrno("Socket::setBlocking: ");
    #endif
}

bool Socket::isValid() const {
    // Check if socket is valid
    #ifdef ROGUELIKE_UNIX
    return rawSock >= 0; // Any negative value is invalid on Unix, not just -1
    #else
    return rawSock != INVALID_SOCKET;
    #endif
}

void Socket::invalidate() {
    // Set socket to invalid value, without close
    rawSock = INVALID_SOCKET;
}

void Socket::close() {
    // Close socket if socket is valid
    if(isValid()) {
        #ifdef ROGUELIKE_UNIX
        int closeResult = ::close(rawSock);
        #else
        int closeResult = ::closesocket(rawSock);
        #endif
        
        if(closeResult == SOCKET_ERROR)
            throw SocketException::fromErrno("Socket::close: ");
        
        // Invalidate socket
        invalidate();
    }
}
