#ifndef ROGUELIKE_SERVER_CONNECTION_HPP_INCLUDED
#define ROGUELIKE_SERVER_CONNECTION_HPP_INCLUDED
#include "../networking/buffer.hpp"
#include <string>

struct ServerConnection {
    Buffer r_buffer, w_buffer;
    std::string player;
};

#endif
