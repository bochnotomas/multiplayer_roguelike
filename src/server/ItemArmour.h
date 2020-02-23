#include "ItemClasses.h"

class ItemArmour : public Item {
  ItemArmour(string name, string type, string desc) :
    Item(name, type, desc, 'U')
  {}

  virtual void extraHealth() = 0;
}
