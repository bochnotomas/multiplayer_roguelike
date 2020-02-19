#ifndef ROGUELIKE_SOCKET_EXCEPTION_HPP_INCLUDED
#define ROGUELIKE_SOCKET_EXCEPTION_HPP_INCLUDED
#include "SocketPlatform.hpp"
#include <exception>
#include <string>

class SocketException : std::exception {
    std::string message;
    
public:
    /// Create exception from message
    SocketException(std::string message);
    
    /// Create exception from errno (factory)
    static SocketException fromErrno(std::string prefix);
    
    /// Create exception from getaddrinfo error (factory)
    static SocketException fromGaiErrno(std::string prefix, int gaiErrno);
    
    const char* what() const noexcept override;
};

#endif
