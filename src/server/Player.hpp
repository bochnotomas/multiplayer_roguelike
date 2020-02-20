#ifndef ROGUELIKE_PLAYER_HPP_INCLUDED
#define ROGUELIKE_PLAYER_HPP_INCLUDED
#include "../networking/Socket.hpp"
#include "../networking/Buffer.hpp"
#include "../client/Object.h"
#include <vector>

enum eDirection { STOP = 0, LEFT, RIGHT, UP, DOWN };

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
    
    // TODO make players explicitly move-only
    
    /// Destructor
    ~Player() = default;

    int playerPositionX, playerPositionY;
    int obstaclePositionX, obstaclePositionY;
    int potionPositionX, potionPositionY;
    int health;
    int attack;
    int defense;
    int strength;
    int speed;
    eDirection dir;

    void potionCheck(int axisValue1, int axisValue2, std::vector<std::vector<char> >& map);

    void playerMovementLogic(std::vector<std::vector<char> >& map);

    void inputHandling(char newDir);
};



#endif
