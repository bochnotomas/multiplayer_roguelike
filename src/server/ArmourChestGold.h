#ifndef ARMOUR_CHEST_GOLD_H
#define ARMOUR_CHEST_GOLD_H
#include "ItemArmour.h"
#include <string>

using namespace std;

class ArmourChestGold : public ItemArmour {
	public:
	ArmourChestGold(std::pair<int, int> start_position) :
        ItemArmour("Gold Chestplate", "CHEST_ARMOUR", "Not only protects the wearer but makes them look good at the same time.", start_position)
    {}

    void equipArmour(Player* player) {
		player->health = player->health * 2;
    }
	
	void unequipArmour(Player* player) {
		player->health = player->health / 2;
    }
};
#endif
