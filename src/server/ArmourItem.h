#include "ItemClasses.h"

class ArmourItem : public Item {
  ArmourItem(string name, string type, string desc) :
    Item(name, type, desc, 'U')
  {}

  virtual void extraHealth() = 0;
}
