#ifndef ARMOUR_CHEST_H
#define ARMOUR_CHEST_H
#include "ItemArmour.h"

using namespace std;

class ArmourChest : public ItemArmour {
	public:
	ArmourChest() : ItemArmour("Armour Chestplate", "CHEST_ARMOUR", "Moulded from the toughest metals found on earth, this will protect the wearer from a lot of things.");
    {
	    
    }

    void extraHealth() {
        std::cout << "You don the armour and become more resilient." << std::endl;
    }
};
#endif
