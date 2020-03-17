#ifndef ROGUELIKE_MENU_ITEM_HPP_INCLUDED
#define ROGUELIKE_MENU_ITEM_HPP_INCLUDED
#include "Renderer.h"

// A menu item. Can be derived from, but it is not necessary to do so
class MenuItem {
protected:
    // Item key, used as an unique identifier. You are encouragedto use an
    // enumerator for this
    const int key;
    
    // Text content of item
    const std::string text;
    
    // Formatting for unselected and selected state
    const Formating formatting, selectedFormatting;
    
    // Whether this menu item can be selected or not
    bool selectable;
public:
    MenuItem(int key, std::string text, bool selectable = true, const Formating& formatting = {Color::WHITE, Color::NO_COLOR}, const Formating& selectedFormatting = {Color::BLACK, Color::WHITE});
    
    virtual ~MenuItem() = default;
    
    // Get key of item
    int getKey() const;
    
    // Get length of item. By default, returns the length of the item's text
    virtual unsigned int getLength() const;
    
    // Check if the item is selectable
    bool isSelectable() const;
    
    // Render the menu item at the given limits and according to whether its
    // selected or not
    virtual void drawAt(Renderer* renderer, int left, int right, int y, bool selected) const;
    
    // On input, this function is called. Returns true if the input was trapped
    // or false if not. By default, this does not trap the input and does
    // nothing
    virtual bool onInput(char input);
};

#endif
