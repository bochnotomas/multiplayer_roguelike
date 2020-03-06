#ifndef POTION_HEALTH_H
#define POTION_HEALTH_H
#include "ItemPotion.h"
#include <string>
#include "Player.hpp"
using namespace std;

class PotionHealth : public ItemPotion {
    public:
    PotionHealth() :
        ItemPotion("Health potion", "HEALTH_POTION", "This gives you health")
    {}

    void drink(Player*player) {
        cout << "You drink the health potion!" << endl;
		player->healthPotionCooldown = 3;
		player->health = 200;
    }
};
#endif
