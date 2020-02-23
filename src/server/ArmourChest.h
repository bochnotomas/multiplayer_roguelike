#include "ItemArmour.h"
using namespace std;

class ArmourChest : public ItemArmour {
  ArmourChest() :
    ItemArmour("Armour Chestplate", "CHEST_ARMOUR", "Moulded from the toughest metals found on earth, this will protect the wearer from a lot of things.", 'U')
  {}

  void extraHealth() {
    cout << "You don the armour and become more resilient." << endl
  }
}
