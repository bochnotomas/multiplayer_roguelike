#ifndef ROGUELIKE_INPUT_MENU_ITEM_HPP_INCLUDED
#define ROGUELIKE_INPUT_MENU_ITEM_HPP_INCLUDED
#include "MenuItem.hpp"

class InputMenuItem : public MenuItem {
    // Current input value in menu item
    std::string value;
    
    // If input is locked or not
    bool locked;
public:
    InputMenuItem(int key, std::string text, std::string defaultValue = "", const Formating& formatting = {Color::WHITE, Color::NO_COLOR}, const Formating& selectedFormatting = {Color::BLACK, Color::WHITE});
    
    // Get current input
    std::string get() const;
    
    // Set input
    void set(std::string value);
    
    // Unlock input
    void unlock();
    
    // Get length of item
    unsigned int getLength() const;
    
    // Draw
    void drawAt(Renderer* renderer, int left, int right, int y, bool selected) const;
    
    // Handle input character
    bool onInput(char input);
};

#endif
