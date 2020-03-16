#ifndef ITEM_WEAPON_H
#define ITEM_WEAPON_H
#include "ItemClasses.h"
#include "Player.hpp"
#include <string>

using namespace std;

class ItemWeapon : public Item {
    public:
    ItemWeapon(string name, string type, string desc, char character, std::pair<int, int> start_position) :
        Item(name, type, desc, character, start_position)
    {}

    virtual void attack() {}; // does nothing by default
    // find the difference between a virutal function and a pure virtual
};
#endif
