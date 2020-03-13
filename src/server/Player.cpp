#include "../networking/Direction.hpp"
#include "Player.hpp"

Player::Player(Socket* socket) :
    Socket(std::move(*socket)), // Call move constructor. Source invalidated
    Object('@'),
    level(0)
{
    dir = eDirection::STOP;
    speed = 1;
    health = 100;
    attack = 1;
    defense = 1;
    strength = 1;
    speedPotionCooldown = 0;
    healthPotionCooldown = 0;
    playerPositionX = 10;
    playerPositionY = 15;
    
    m_type = ObjectType::PLAYER;
}

Player::~Player() {
    if(isValid()) {
        try {
            // Shutdown socket completely
            shutdown(SocketShutdownMode::ShutReadWrite);
        }
        catch(SocketException e) {}; // Ignore network exceptions
    }
}

void Player::potionCheck(int axisValue1, int axisValue2, std::vector<std::vector<char>>& map) {
    if (map[axisValue1][axisValue2] == 'P') {
        speed++;
    }
}

void Player::playerMovementLogic(std::vector<std::vector<char>>& map) {
    bool ifBreak = false;
    switch (dir) {
    case eDirection::LEFT:
        for (int i = playerPositionX; i >= (playerPositionX - speed); i--) {
            if (map[playerPositionY][i] == '#') {
                playerPositionX = (i + 1);
                ifBreak = true;
                break;
            }
            potionCheck(playerPositionY, i, map);

        }
        if (ifBreak == false) {
            playerPositionX = playerPositionX - speed;
        }
        dir = eDirection::STOP;
        break;
    case eDirection::RIGHT:
        for (int i = playerPositionX; i <= (playerPositionX + speed); i++) {
            if (map[playerPositionY][i] == '#') {

                playerPositionX = (i - 1);
                ifBreak = true;
                break;
            }
            potionCheck(playerPositionY, i, map);

        }
        if (ifBreak == false) {
            playerPositionX = playerPositionX + speed;
        }
        dir = eDirection::STOP;
        break;
    case eDirection::UP:
        for (int i = playerPositionY; i >= (playerPositionY - speed); i--) {
            if (map[i][playerPositionX] == '#') {
                playerPositionY = (i + 1);
                ifBreak = true;
                break;
            }
            potionCheck(i, playerPositionX, map);
        }
        if (ifBreak == false) {

            playerPositionY = playerPositionY - speed;
        }
        dir = eDirection::STOP;
        break;
    case eDirection::DOWN:
        for (int i = playerPositionY; i <= (playerPositionY + speed); i++) {
            if (map[i][playerPositionX] == '#') {
                playerPositionY = (i - 1);
                ifBreak = true;
                break;
            }
            potionCheck(i, playerPositionX, map);
        }
        if (ifBreak == false) {
            playerPositionY = playerPositionY + speed;
        }
        dir = eDirection::STOP;
        break;
    }

    if (playerPositionX > 38) {
        playerPositionX = 38;
    }
    if (playerPositionX < 1) {
        playerPositionX = 1;
    }
    if (playerPositionY > 18) {
        playerPositionY = 18;
    }
    if (playerPositionY < 1) {
        playerPositionY = 1;
    }

    if (speed < 1) {
        speed = 1;
    }
}


void Player::inputHandling(char newDir)
{
    switch (newDir) {
    case 'a':
        dir = eDirection::LEFT;
        break;
    case 's':
        dir = eDirection::DOWN;
        break;
    case 'd':
        dir = eDirection::RIGHT;
        break;
    case 'w':
        dir = eDirection::UP;
        break;
    case '-':
        speed--;
        break;
    }
}


