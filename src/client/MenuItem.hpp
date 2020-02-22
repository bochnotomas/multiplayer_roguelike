#ifndef ROGUELIKE_MENU_ITEM_HPP_INCLUDED
#define ROGUELIKE_MENU_ITEM_HPP_INCLUDED
#include "Renderer.h"

// A menu item. Can be derived from, but it is not necessary to do so
class MenuItem {
    // Item key, used as an unique identifier. You are encouragedto use an
    // enumerator for this
    const int key;
    
    // Text content of item
    const std::string text;
    
    // Formatting for unselected and selected state
    const Formating formatting, selectedFormatting;
public:
    MenuItem(int key, std::string text, const Formating& formatting = {Color::WHITE, Color::NO_COLOR}, const Formating& selectedFormatting = {Color::BLACK, Color::WHITE});
    
    virtual ~MenuItem() = default;
    
    // Get key of item
    int getKey() const;
    
    // Get length of item. By default, returns the length of the item's text
    unsigned int getLength() const;
    
    // Render the menu item at the given limits and according to whether its
    // selected or not
    virtual void drawAt(Renderer* renderer, int left, int right, int y, bool selected) const;
};

#endif
