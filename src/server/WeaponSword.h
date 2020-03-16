#ifndef WEAPON_SWORD_H
#define WEAPON_SWORD_H
#include "ItemWeapon.h"
#include <string>
using namespace std;

class WeaponSword : public ItemWeapon {
    public:
    WeaponSword(std::pair<int, int> start_position) :
        ItemWeapon("Sword", "SWORD_WEAPON", "A noble blade, created by the ancient blacksmiths many years ago.", '!', start_position)
    {}

    void attack() {
        // TODO
    }
};
#endif
