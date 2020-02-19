#include "SocketException.hpp"

#ifdef ROGUELIKE_UNIX
    #include <cerrno> // errno
    #include <cstring> // strerror
    #include <netdb.h> // gai_strerror
#else
    #include <winsock2.h>

    /// Convert winsock2 error code to a human-readable string
    std::string winsock2ErrorToString(int error) {
        // Get error message
        char* errorCStr = nullptr;
        FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, 
                    nullptr,
                    error,
                    MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                    (LPSTR)&errorCStr,
                    0,
                    nullptr);
        
        // Convert to string
        std::string errorStr(errorCStr);
        
        // Free allocated C-string
        LocalFree(errorCStr);
        
        return errorStr;
    }
#endif

SocketException::SocketException(std::string message) :
    message(message)
{}

SocketException SocketException::fromErrno(std::string prefix) {
    #ifdef ROGUELIKE_UNIX
    return SocketException(prefix + strerror(SOCKET_LAST_ERROR));
    #else
    return SocketException(prefix + winsock2ErrorToString(SOCKET_LAST_ERROR));
    #endif
}

SocketException SocketException::fromGaiErrno(std::string prefix, int gaiErrno) {
    #ifdef ROGUELIKE_UNIX
    if(gaiErrno == EAI_SYSTEM)
        return SocketException::fromErrno(prefix);
    else
        return SocketException(prefix + gai_strerror(gaiErrno));
    #else
    return SocketException(prefix + winsock2ErrorToString(gaiErrno));
    #endif
}

const char * SocketException::what() const noexcept {
    return message.c_str();
}
