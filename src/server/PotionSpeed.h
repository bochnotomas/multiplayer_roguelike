#ifndef POTION_SPEED_H
#define POTION_SPEED_H
#include "ItemPotion.h"
#include <string>
using namespace std;

class PotionSpeed : public ItemPotion {
    public:
    PotionSpeed(std::pair<int, int> start_position) :
        ItemPotion("Speed potion", "SPEED_POTION", "This gives you faster movement speed", start_position)
    {}

    void drink(Player* player) {
		player->speedPotionCooldown = 3;
		player->speed = 2;
    }
};
#endif
