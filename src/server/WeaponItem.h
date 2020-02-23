#include "ItemClasses.h"

class WeaponItem : public Item {
  WeaponItem(string name, string type, string desc, char character) :
    Item(name, type, desc, character)
  {}

  virtual void attack() = 0; // pure virtual function
  // find the difference between a virutal function and a pure virtual
}
