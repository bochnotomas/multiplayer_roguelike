#ifndef INVENTORY_H
#define INVENTORY_H
#include <vector>
#include <string>
#include "ItemClasses.h"

using namespace std;

class Inventory
{
	public:
	vector<Item>inventory;
	
	void pickUp(Item itemFromMap){
		inventory.push_back(itemFromMap);
	}
	
	//void dropItem(itemFromMap){
	//
	//}
};
#endif
