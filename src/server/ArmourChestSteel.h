#ifndef ARMOUR_CHEST_STEEL_H
#define ARMOUR_CHEST_STEEL_H
#include "ItemArmour.h"
#include <string>

using namespace std;

class ArmourChestSteel : public ItemArmour {
	public:
	ArmourChestSteel() : ItemArmour("Steel Chestplate", "CHEST_ARMOUR_STEEL", "Moulded from the toughest metals found on earth, this will protect the wearer from a lot of things.");
    {
	    
    }

    void equipArmour(Player*player) {
        std::cout << "You don the armour and become more resilient." << std::endl;
		player->health = health * 1.5;
    }
	
	void unequipArmour(Player*player) {
		std::cout << "You have taken off the armour" << std::endl;
		player->health = health / 1.5;
};
#endif
