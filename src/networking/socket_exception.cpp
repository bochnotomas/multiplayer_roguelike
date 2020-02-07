#include "socket_exception.hpp"

SocketException::SocketException(std::string message) :
    message(message)
{}

const char * SocketException::what() const noexcept {
    return message.c_str();
}
