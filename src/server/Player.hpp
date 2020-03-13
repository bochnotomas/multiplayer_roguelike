#ifndef ROGUELIKE_PLAYER_HPP_INCLUDED
#define ROGUELIKE_PLAYER_HPP_INCLUDED
#include "../networking/Direction.hpp"
#include "../networking/Socket.hpp"
#include "../networking/Buffer.hpp"
#include "Object.h"
#include "Map.h"
#include <vector>

/// A player that is connected to a server. A player _IS_ a socket, since it
/// cannot exist without a connection
struct Player : Socket, Object {
    /// Read/write buffers for network messages
    Buffer rBuffer, wBuffer;
    
    /// The player's name. If empty, they haven't joined yet
    std::string name;
    
    /// Constructor. Needs a socket. The socket is moved to the player, so the
    /// original instance is invalidated (as in, the source Socket's raw socket
    /// is invalidated, the source Socket instance is not destroyed)
    Player(Socket* socket);
    
    /// Destructor
    ~Player();

    int health;
    int attack;
    int defense;
    int strength;
    int speed;
    int speedPotionCooldown;
    int healthPotionCooldown;
    eDirection dir;
    
    int level;

    void itemCheck(int axisValue1, int axisValue2, Map& map);

    void playerMovementLogic(Map& map);

    void inputHandling(char newDir);
};



#endif
