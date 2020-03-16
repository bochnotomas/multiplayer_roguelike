#ifndef ITEM_ARMOUR_H
#define ITEM_ARMOUR_H
#include "ItemClasses.h"
#include "Player.hpp"
#include <string>

using namespace std;

class ItemArmour : public Item {
    public:
    ItemArmour(string name, string type, string desc, std::pair<int, int> start_position) :
        Item(name, type, desc, 'U', start_position)
    {}

    virtual void equipArmour(Player* player) {}; // Does nothing by default
	virtual void unequipArmour(Player* player) {};
};
#endif
