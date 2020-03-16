#ifndef ARMOUR_CHEST_STEEL_H
#define ARMOUR_CHEST_STEEL_H
#include "ItemArmour.h"
#include <string>

using namespace std;

class ArmourChestSteel : public ItemArmour {
	public:
	ArmourChestSteel(std::pair<int, int> start_position) :
        ItemArmour("Steel Chestplate", "CHEST_ARMOUR_STEEL", "Moulded from the toughest metals found on earth, this will protect the wearer from a lot of things.", start_position)
    {}

    void equipArmour(Player* player) {
		player->health = player->health * 1.5;
    }
	
	void unequipArmour(Player* player) {
		player->health = player->health / 1.5;
    }
};
#endif
