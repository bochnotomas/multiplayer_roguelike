#ifndef ROGUELIKE_ITEM_CLASSES_H_INCLUDED
#define ROGUELIKE_ITEM_CLASSES_H_INCLUDED
#include <string>
#include "Object.h"

class Item : public Object
{
  //making it public (i think????)
  public:

  //creating the attributes
  std::string itemName; //name of the item
  std::string itemType; //the type of item it is i.e. weapon, armour etc
  std::string itemDesc; //small description of the item the player can pull up
  char itemChar; //the character that will represent the item on the map

  Item(std::string m_itemName, std::string m_itemType, std::string m_itemDesc, char m_itemChar, std::pair<int, int> start_position) :
    itemName(m_itemName), // initializer list
    itemType(m_itemType),
    itemDesc(m_itemDesc),
    //itemChar(m_itemChar),
	Object(m_itemChar, Direction::NORTH, true, start_position, {Color::WHITE, Color::GREEN}, {}, ObjectType::ITEM)
  {}
};
/*
int objectCreation()
{
  Item weaponSword("Sword", "WEAPON", "A noble blade, created by the ancient blacksmiths many years ago.", '!'); //create sword object
  Item weaponBow("Bow", "WEAPON", "The weapon of choice for those who wish to attack from range.", 'D'); //create bow object
  Item gearVest("Armour Chestplate", "ARMOUR", "Moulded from the toughest metals found on earth, this will protect the wearer from a lot of things.", 'U'); //create an armour vest object
  Item potionSpeed("Speed potion", "POTION", "One sip of this potion will make the drinker move faster than normal.", 'F'); //create speed potion object
  Item potionHealth("Health potion", "POTION", "A little drop on the tongue will make the drinker become more resilient from enemy attacks.", 'H'); //create health potion object
  Item ammoBow("Arrow", "AMMO", "When fired, these arrows fly through the air with such finesse until they strike their target, at which point they will inflict an ungodly amount of damage.", '/'); //create arrow object

  return 0;
}*/

#endif
