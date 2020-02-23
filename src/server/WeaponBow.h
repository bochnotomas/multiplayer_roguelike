#include "ItemWeapon.h"
using namespace std;

class WeaponBow : public ItemWeapon {
  WeaponBow() :
    ItemWeapon("Bow", "BOW_WEAPON", "The weapon of choice for those who wish to attack from range.", 'D')
  {}

  void attack() {
    cout << "You pull the bow back and fire." << endl
  }
}
