#include "ItemPotion.h"
using namespace std;

class PotionHealth : public ItemPotion {
  PotionHealth() :
    ItemPotion("Health potion", "HEALTH_POTION", "This gives you health")
  {}

  void drink() {
    cout << "You drink the health potion!" << endl
  }
}
