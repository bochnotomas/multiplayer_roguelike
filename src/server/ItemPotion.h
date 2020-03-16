#ifndef ITEM_POTION_H
#define ITEM_POTION_H
#include "ItemClasses.h"
#include "Player.hpp"
#include <string>

using namespace std;

class ItemPotion : public Item {
    public:
    ItemPotion(string name, string type, string desc, std::pair<int, int> start_position) :
        Item(name, type, desc, 'P', start_position)
    {}

    virtual void drink(Player* player) {}; // does nothing by default
    // find the difference between a virutal function and a pure virtual
};
#endif
