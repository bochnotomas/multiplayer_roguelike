#ifndef ITEM_POTION_H
#define ITEM_POTION_H
#include "ItemClasses.h"
#include <string>

using namespace std;

class ItemPotion : public Item {
    public:
    ItemPotion(string name, string type, string desc) :
        Item(name, type, desc, 'P')
    {}

    virtual void drink() = 0; // pure virtual function
    // find the difference between a virutal function and a pure virtual
};
#endif
