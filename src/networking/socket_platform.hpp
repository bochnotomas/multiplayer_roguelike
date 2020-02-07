#ifndef ROGUELIKE_SOCKET_PLATFORM_HPP_INCLUDED
#define ROGUELIKE_SOCKET_PLATFORM_HPP_INCLUDED

#if defined(unix) || defined(__unix) || defined(__unix__)
    #define ROGUELIKE_SOCKET_UNIX
#else // TODO winsocks2
    #error Unsupported platform. Unix-like only
#endif

#endif
