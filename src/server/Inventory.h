#ifndef INVENTORY_H
#define INVENTORY_H
#include <vector>
#include <string>
#include "ItemArmour.h"
#include "ItemWeapon.h"

using namespace std;

class Inventory
{
	public:
	vector<Item>inventory;
	ItemArmour armourSlot;
	ItemWeapon weaponSlot;
	
	void pickUp(itemFromMap){
		inventory.push_back(itemFromMap);
	}
};
#endif