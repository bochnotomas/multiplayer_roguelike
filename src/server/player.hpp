#ifndef ROGUELIKE_PLAYER_HPP_INCLUDED
#define ROGUELIKE_PLAYER_HPP_INCLUDED
#include "../networking/socket.hpp"
#include "../networking/buffer.hpp"
#include <vector>

using namespace std;
`

enum eDirection { STOP = 0, LEFT, RIGHT, UP, DOWN };

/// A player that is connected to a server. A player _IS_ a socket, since it
/// cannot exist without a connection
struct Player : Socket {
    /// Read/write buffers for network messages
    Buffer r_buffer, w_buffer;
    
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
    int playerSpeed;

    
    eDirection dir;

    void potionCheck(int axisValue1, int axisValue2, vector <vector<char>>& map );

    void playerMovementLogic(vector <vector<char>>& map);

    void inputHandling(char new_dir);
};



#endif
