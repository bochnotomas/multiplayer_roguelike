#include "socket_exception.hpp"

#ifdef ROGUELIKE_UNIX
    #include <cerrno> // errno
    #include <cstring> // strerror
    #include <netdb.h> // gai_strerror
#else
    #include <winsock2.h>

    /// Convert winsock2 error code to a human-readable string
    std::string winsock2_error_to_string(int error) {
        // Get error message
        char* error_c_str = nullptr;
        FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, 
                    nullptr,
                    error,
                    MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                    (LPSTR)&error_c_str,
                    0,
                    nullptr);
        
        // Convert to string
        std::string error_str(error_c_str);
        
        // Free allocated C-string
        LocalFree(error_c_str);
        
        return error_str;
    }
#endif

SocketException::SocketException(std::string message) :
    message(message)
{}

SocketException SocketException::from_errno(std::string prefix) {
    #ifdef ROGUELIKE_UNIX
    return SocketException(prefix + strerror(SOCKET_LAST_ERROR));
    #else
    return SocketException(prefix + winsock2_error_to_string(SOCKET_LAST_ERROR));
    #endif
}

SocketException SocketException::from_gai_errno(std::string prefix, int gai_errno) {
    #ifdef ROGUELIKE_UNIX
    if(gai_errno == EAI_SYSTEM)
        return SocketException::from_errno(prefix);
    else
        return SocketException(prefix + gai_strerror(gai_errno));
    #else
    return SocketException(prefix + winsock2_error_to_string(gai_errno));
    #endif
}

const char * SocketException::what() const noexcept {
    return message.c_str();
}
