#ifndef ARMOUR_CHEST_LEATHER_H
#define ARMOUR_CHEST_LEATHER_H
#include "ItemArmour.h"
#include <string>

using namespace std;

class ArmourChestLeather : public ItemArmour {
	public:
	ArmourChestLeather() : ItemArmour("Leather Chestplate", "CHEST_ARMOUR", "Made from the local cow.");
    {
	    
    }

    void equipArmour(Player*player) {
        std::cout << "You don the armour and become more resilient." << std::endl;
		player->health = health * 1.2;
    }
	
	void unequipArmour(Player*player) {
		std::cout << "You have taken off the armour" << std::endl;
		player->health = health / 1.2;
};
#endif
