#ifndef POTION_HEALTH_H
#define POTION_HEALTH_H
#include "ItemPotion.h"
using namespace std;

class PotionHealth : public ItemPotion {
    public:
    PotionHealth() :
        ItemPotion("Health potion", "HEALTH_POTION", "This gives you health")
    {}

    void drink() {
        cout << "You drink the health potion!" << endl;
    }
};
#endif
