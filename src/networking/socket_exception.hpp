#ifndef ROGUELIKE_SOCKET_EXCEPTION_HPP_INCLUDED
#define ROGUELIKE_SOCKET_EXCEPTION_HPP_INCLUDED
#include <exception>
#include <string>

class SocketException : std::exception {
    std::string message;
    
public:
    SocketException(std::string message);
    
    const char* what() const noexcept override;
};

#endif
