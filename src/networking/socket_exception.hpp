#ifndef ROGUELIKE_SOCKET_EXCEPTION_HPP_INCLUDED
#define ROGUELIKE_SOCKET_EXCEPTION_HPP_INCLUDED
#include "socket_platform.hpp"
#include <exception>
#include <string>

class SocketException : std::exception {
    std::string message;
    
public:
    /// Create exception from message
    SocketException(std::string message);
    
    /// Create exception from errno (factory)
    static SocketException from_errno(std::string prefix);
    
    /// Create exception from getaddrinfo error (factory)
    static SocketException from_gai_errno(std::string prefix, int gai_errno);
    
    const char* what() const noexcept override;
};

#endif
