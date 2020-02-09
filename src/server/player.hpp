#ifndef ROGUELIKE_PLAYER_HPP_INCLUDED
#define ROGUELIKE_PLAYER_HPP_INCLUDED
#include "../networking/socket_platform.hpp"
#include "../networking/buffer.hpp"

struct Player {
    /// Read/write buffers for network messages
    Buffer r_buffer, w_buffer;
    
    /// Network socket for this player
    #ifdef ROGUELIKE_SOCKET_UNIX
    int socket;
    #endif
    
    /// The player's name. If empty, they haven't joined yet
    std::string name;
    
    /// Constructor. Needs a socket
    #ifdef ROGUELIKE_SOCKET_UNIX
    Player(int socket);
    #endif
    
    /// Destructor. Closes socket if not already closed
    ~Player();
};

#endif
