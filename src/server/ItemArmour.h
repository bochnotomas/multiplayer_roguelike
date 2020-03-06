#ifndef ITEM_ARMOUR_H
#define ITEM_ARMOUR_H
#include "ItemClasses.h"

using namespace std;

class ItemArmour : public Item {
    public:
    ItemArmour(string name, string type, string desc) :
        Item(name, type, desc, 'U')
    {}

    virtual void extraHealth() = 0;
};
#endif
