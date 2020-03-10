#ifndef POTION_SPEED_H
#define POTION_SPEED_H
#include "ItemPotion.h"
#include <string>
#include "Player.hpp"
using namespace std;

class PotionSpeed : public ItemPotion {
    public:
    PotionSpeed() :
        ItemPotion("Speed potion", "SPEED_POTION", "This gives you faster movement speed")
    {}

    void drink(Player*player) {
        cout << "You drink the speed potion!" << endl;
		player->speedPotionCooldown = 3;
		player->speed = 2;
    }
};
#endif
