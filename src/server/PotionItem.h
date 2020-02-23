#include "ItemClasses.h"

class PotionItem : public Item {
  PotionItem(string name, string type, string desc) :
    Item(name, type, desc, 'P')
  {}

  virtual void drink() = 0; // pure virtual function
  // find the difference between a virutal function and a pure virtual
}
