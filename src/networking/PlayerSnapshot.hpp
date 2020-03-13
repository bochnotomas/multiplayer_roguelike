#ifndef ROGUELIKE_PLAYER_SNAPSHOT_HPP_INCLUDED
#define ROGUELIKE_PLAYER_SNAPSHOT_HPP_INCLUDED
#include <string>

struct PlayerSnapshot {
    std::string name;
    int x;
    int y;
    int level;
    
    PlayerSnapshot(std::string name, int x, int y, int level) :
        name(name),
        x(x),
        y(y),
        level(level)
    {}
};

#endif
