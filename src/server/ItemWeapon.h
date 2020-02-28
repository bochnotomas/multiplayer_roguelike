#ifndef ITEM_WEAPON_H
#define ITEM_WEAPON_H
#include "ItemClasses.h"

class ItemWeapon : public Item {
    public:
    ItemWeapon(string name, string type, string desc, char character) :
        Item(name, type, desc, character)
    {}

    virtual void attack() = 0; // pure virtual function
    // find the difference between a virutal function and a pure virtual
};
#endif
