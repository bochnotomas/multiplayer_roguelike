#ifndef ROGUELIKE_PLAYER_SNAPSHOT_HPP_INCLUDED
#define ROGUELIKE_PLAYER_SNAPSHOT_HPP_INCLUDED
#include <string>

struct PlayerSnapshot {
    std::string name;
    int x;
    int y;
    int level;
    std::vector<std::string> items;
    
    PlayerSnapshot(std::string name, int x, int y, int level, std::vector<std::string> items) :
        name(name),
        x(x),
        y(y),
        level(level),
        items(items)
    {}
};

#endif
