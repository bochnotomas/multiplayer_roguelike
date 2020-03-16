#ifndef WEAPON_BOW_H
#define WEAPON_BOW_H
#include "ItemWeapon.h"
#include <string>
using namespace std;

class WeaponBow : public ItemWeapon {
    public:
    WeaponBow(std::pair<int, int> start_position) :
        ItemWeapon("Bow", "BOW_WEAPON", "The weapon of choice for those who wish to attack from range.", 'D', start_position)
    {}

    void attack() {
        // TODO
    }
};
#endif
