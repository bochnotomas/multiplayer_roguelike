#include "player.hpp"

Player::Player(Socket* socket) :
    Socket(std::move(*socket)) // Call move constructor. Source invalidated
{}
