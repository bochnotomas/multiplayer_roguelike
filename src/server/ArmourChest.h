#ifndef ARMOUR_CHEST_H
#define ARMOUR_CHEST_H
#include "ItemArmour.h"
#include <string>

using namespace std;

class ArmourChest : public ItemArmour {
	public:
	ArmourChest() : ItemArmour("Armour Chestplate", "CHEST_ARMOUR", "Moulded from the toughest metals found on earth, this will protect the wearer from a lot of things.");
    {
	    
    }

    void equipArmour(Player*player) {
        std::cout << "You don the armour and become more resilient." << std::endl;
		player->health = health * 2;
    }
	
	void unequipArmour(Player*player) {
		std::cout << "You have taken off the armour" << std::endl;
		player->health = health * 0.5;
};
#endif
