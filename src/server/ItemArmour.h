#ifndef ITEM_ARMOUR_H
#define ITEM_ARMOUR_H
#include "ItemClasses.h"
#include <string>

using namespace std;

class ItemArmour : public Item {
    public:
    ItemArmour(string name, string type, string desc) :
        Item(name, type, desc, 'U')
    {}

    virtual void equipArmour() = 0;
	virtual void unequipArmour() = 0;
};
#endif
