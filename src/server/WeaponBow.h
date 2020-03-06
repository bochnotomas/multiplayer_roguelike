#ifndef WEAPON_BOW_H
#define WEAPON_BOW_H
#include "ItemWeapon.h"
#include <string>
using namespace std;

class WeaponBow : public ItemWeapon {
    public:
    WeaponBow() :
        ItemWeapon("Bow", "BOW_WEAPON", "The weapon of choice for those who wish to attack from range.", 'D')
    {}

    void attack() {
        cout << "You pull the bow back and fire." << endl;
    }
};
#endif
