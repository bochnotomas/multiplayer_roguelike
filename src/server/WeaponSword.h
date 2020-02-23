#include "ItemWeapon.h"
using namespace std;

class WeaponSword : public ItemWeapon {
    WeaponSword() :
        ItemWeapon("Sword", "SWORD_WEAPON", "A noble blade, created by the ancient blacksmiths many years ago.", '!')
    {}

    void attack() {
        cout << "You swing your sword." << endl;
    }
};
