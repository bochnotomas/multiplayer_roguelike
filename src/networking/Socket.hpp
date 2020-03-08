#ifndef ROGUELIKE_SOCKET_HPP_INCLUDED
#define ROGUELIKE_SOCKET_HPP_INCLUDED
#include "SocketException.hpp"
#include <cstdint>
#include <vector>
#include <memory>

#ifdef ROGUELIKE_UNIX
    #include <sys/socket.h> // socket, bind, AF_INET
    #include <arpa/inet.h> // sockaddr_in, sockaddr, htons, INADDR_ANY, IPPROTO_TCP
    #include <sys/types.h> // AF_INET (Required for BSD systems only)
#else
    #include <winsock2.h>
    #include <ws2tcpip.h>
#endif

enum SocketShutdownMode {
    ShutRead,
    ShutWrite,
    ShutReadWrite
};

/// A cross platform socket object
class Socket {
    SOCKET rawSock = INVALID_SOCKET;
    
    /// Create socket from raw socket. This is private for safety
    Socket(SOCKET rawSock);
    
    /// Write data. Returns number of bytes written. Can block if the socket is
    /// not in non-blocking mode. WARNING: This will signal a SIGPIPE if you
    /// try to write data after the socket has been closed (Unix-only)
    size_t write(const uint8_t* data, size_t dataSize);
    
    /// SocketSelector is a friend of Socket
    friend class SocketSelector;
public:
    /// Initialise socket API. Only used for winsock2 if on Windows
    static void initSocketApi();
    
    /// Cleanup socket API
    static void cleanupSocketApi();
    
    /// Resolves a host to a list of addresses. If internet access is down,
    /// this _WILL_ block forever
    // TODO make this non-blocking using a dedicated thread
    static std::vector<IN_ADDR> resolve(std::string host);
    
    /// Create socket with address family, socket type and protocol
    Socket(SOCKET_ADDRESS_FAMILY addressFamily, int type, int protocol);
    
    /// Create invalid socket (default constructor)
    Socket();
    
    /// Sockets can be moved and move assigned
    Socket(Socket&& other);
    Socket& operator=(Socket&& other);
    
    /// Sockets cannot be copied or copy-assigned
    Socket(const Socket&) = delete;
    void operator=(const Socket&) = delete;
    
    /// Cleanup. Virtual in case classes want to inherit (see Player)
    virtual ~Socket();
    
    /// Compare 2 sockets together. If they have the same raw socket, they are
    /// the same
    bool operator==(const Socket& other) const;
    
    /// Read data to output. Returns whether the connection is still connected.
    /// Can block if the socket is not in non-blocking mode
    bool read(std::vector<uint8_t>& output);
    
    /// Same as write (see private write above), but for a whole byte vector
    size_t write(const std::vector<uint8_t>& data);
    
    /// Same as above, but for vector iterators. Assumes iterators are valid
    size_t write(const std::vector<uint8_t>::iterator begin, const std::vector<uint8_t>::iterator end);
    
    /// Same as above, but for vector iterator and size. Assumes iterator is
    /// valid
    size_t write(const std::vector<uint8_t>::iterator begin, size_t dataSize);
    
    /// Bind socket to address family, address and port
    void bind(SOCKET_ADDRESS_FAMILY addressFamily, IN_ADDR address, uint16_t port);
    
    /// Bind socket to address family and port for all addresses
    void bind(SOCKET_ADDRESS_FAMILY addressFamily, uint16_t port);
    
    /// Mark socket as listening for connections. backlog is the maximum amount
    /// of pending connections
    void listen(int backlog);
    
    /// Accept a pending incoming connection. Returns nullptr if there is no
    /// pending connection and the socket is non-blocking, otherwise, a pointer
    /// to a new Socket instance. Note that this can block if the socket is not
    /// non-blocking
    std::unique_ptr<Socket> accept();
    
    /// Connect to an address family, address and port. Returns true if the
    /// connection was successful, false otherwise, on non-blocking calls. On
    /// blocking calls, false never gets returned, as the only way this can
    /// fail is by throwing a SocketException
    bool connect(SOCKET_ADDRESS_FAMILY addressFamily, IN_ADDR address, uint16_t port);
    
    /// Shutdown the socket, flagging that you will no longer read data, write
    /// data, or both;
    void shutdown(SocketShutdownMode mode);
    
    /// Set socket to blocking or non-blocking mode. Sockets are blocking by
    /// default
    void setBlocking(bool blocking);
    
    /// Check if the socket is valid
    bool isValid() const;
    
    /// Invalidate the socket without closing it. Only call this if you know
    /// what you are doing, e.g. call it if poll() reports that this socket has
    /// already been closed
    void invalidate();
    
    /// Close the socket (if valid)
    void close();
};

#endif
