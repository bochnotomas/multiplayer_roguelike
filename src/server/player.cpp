#include "player.hpp"

#ifdef ROGUELIKE_SOCKET_UNIX
    #include <unistd.h> // close
#endif

Player::Player(int socket) :
    socket(socket)
{}

Player::~Player() {
    #ifdef ROGUELIKE_SOCKET_UNIX
    
    if(socket != -1)
        close(socket);
    
    #endif
}
