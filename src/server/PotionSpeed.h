#include "ItemPotion.h"
using namespace std;

class PotionSpeed : public ItemPotion {
  PotionSpeed() :
    ItemPotion("Speed potion", "SPEED_POTION", "This gives you faster movement speed")
  {}

  void drink() {
    cout << "You drink the speed potion!" << endl
  }
}
