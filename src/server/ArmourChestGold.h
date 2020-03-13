#ifndef ARMOUR_CHEST_GOLD_H
#define ARMOUR_CHEST_GOLD_H
#include "ItemArmour.h"
#include <string>

using namespace std;

class ArmourChestGold : public ItemArmour {
	public:
	ArmourChestGold() : ItemArmour("Gold Chestplate", "CHEST_ARMOUR", "Not only protects the wearer but makes them look good at the same time.");
    {
	    
    }

    void equipArmour(Player*player) {
        std::cout << "You don the armour and become more resilient." << std::endl;
		player->health = health * 2;
    }
	
	void unequipArmour(Player*player) {
		std::cout << "You have taken off the armour" << std::endl;
		player->health = health / 2;
};
#endif