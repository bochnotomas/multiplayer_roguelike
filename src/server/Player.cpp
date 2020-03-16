#include "../networking/Direction.hpp"
#include "Player.hpp"
#include "Inventory.h"

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

void Player::itemCheck(Map& map) {
	for(auto it = map.objects.begin(); it != map.objects.end();)
	{
		std::pair<int, int> positionOfObject = get_position(*it);
		std::pair<int, int> positionOfPlayer = m_position;
		if( positionOfObject == positionOfPlayer && (*it).get_type() == ObjectType::ITEM) {
			pickUp(*it);
			it = map.objects.erase(it)
		}
		else
			it++;
	}
    
}

void Player::playerMovementLogic(Map& map) {
    bool ifBreak = false;
    switch (dir) {
    case eDirection::LEFT:
        for (int i = m_position.first; i >= (m_position.first - speed); i--) {
            if (!(*map.get_map_plane())[m_position.second][i].accesible) {
                m_position.first = (i + 1);
                ifBreak = true;
                break;
            }
            itemCheck(m_position.second, i, map);

        }
        if (ifBreak == false) {
            m_position.first = m_position.first - speed;
        }
        dir = eDirection::STOP;
        break;
    case eDirection::RIGHT:
        for (int i = m_position.first; i <= (m_position.first + speed); i++) {
            if (!(*map.get_map_plane())[m_position.second][i].accesible) {

                m_position.first = (i - 1);
                ifBreak = true;
                break;
            }
            itemCheck(m_position.second, i, map);

        }
        if (ifBreak == false) {
            m_position.first = m_position.first + speed;
        }
        dir = eDirection::STOP;
        break;
    case eDirection::UP:
        for (int i = m_position.second; i >= (m_position.second - speed); i--) {
            if (!(*map.get_map_plane())[i][m_position.first].accesible) {
                m_position.second = (i + 1);
                ifBreak = true;
                break;
            }
            itemCheck(i, m_position.first, map);
        }
        if (ifBreak == false) {

            m_position.second = m_position.second - speed;
        }
        dir = eDirection::STOP;
        break;
    case eDirection::DOWN:
        for (int i = m_position.second; i <= (m_position.second + speed); i++) {
            if (!(*map.get_map_plane())[i][m_position.first].accesible) {
                m_position.second = (i - 1);
                ifBreak = true;
                break;
            }
            itemCheck(i, m_position.first, map);
        }
        if (ifBreak == false) {
            m_position.second = m_position.second + speed;
        }
        dir = eDirection::STOP;
        break;
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


