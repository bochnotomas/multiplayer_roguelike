#ifndef ARMOUR_CHEST_LEATHER_H
#define ARMOUR_CHEST_LEATHER_H
#include "ItemArmour.h"
#include <string>

using namespace std;

class ArmourChestLeather : public ItemArmour {
	public:
	ArmourChestLeather(std::pair<int, int> start_position) :
        ItemArmour("Leather Chestplate", "CHEST_ARMOUR", "Made from the local cow.", start_position)
    {}

    void equipArmour(Player* player) {
		player->health = player->health * 1.2;
    }
	
	void unequipArmour(Player* player) {
		player->health = player->health / 1.2;
    }
};
#endif
