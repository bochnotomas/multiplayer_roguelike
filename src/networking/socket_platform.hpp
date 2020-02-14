#ifndef ROGUELIKE_SOCKET_PLATFORM_HPP_INCLUDED
#define ROGUELIKE_SOCKET_PLATFORM_HPP_INCLUDED

#if defined(unix) || defined(__unix) || defined(__unix__)
    #define ROGUELIKE_UNIX
    
    // For compatibility with winsock2
    #define SOCKET int
    #define INVALID_SOCKET -1
    #define SOCKET_ERROR -1
    
    // For compatibility with Berkeley sockets
    #define SOCKET_ADDRESS_FAMILY sa_family_t
    #define SOCKET_EAGAIN EAGAIN
    #define SOCKET_EWOULDBLOCK EAGAIN
    #define SOCKET_LAST_ERROR errno
    #define IN_ADDR in_addr
#elif defined(_WIN32) || defined(WIN32)
    // For compatibility with Berkeley sockets
    #define SOCKET_ADDRESS_FAMILY short
    #define SOCKET_EAGAIN WSAEAGAIN
    #define SOCKET_EWOULDBLOCK WSAEAGAIN
    #define SOCKET_LAST_ERROR WSAGetLastError()
#else
    #error Unsupported platform. Unix-like and Windows only
#endif

#endif
