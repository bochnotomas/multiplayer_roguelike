#ifndef ROGUELIKE_MENU_HPP_INCLUDED
#define ROGUELIKE_MENU_HPP_INCLUDED
#include "Renderer.h"
#include <mutex>

const Formating defaultItemFormatting = {
    Color::WHITE,
    Color::NO_COLOR
};

const Formating defaultSelectedItemFormatting = {
    Color::BLACK,
    Color::WHITE
};

// Menu class. Called by Renderer
class Menu : public Drawable {
    // The items in the menu
    std::vector<std::string> items;
    
    // Longest item length. Used for optimisation purposes
    unsigned int longestLength;
    
    // Current selection. Never >= items.size(), unless there are no items
    unsigned int selection;
    
    // Prevent changing a menu selected when a frame is being rendered
    std::mutex selectionLock;
public:
    Menu(unsigned int width, unsigned int height, int xOffset = 0, int yOffset = 0, Formating formatting = defaultItemFormatting, Formating selectedFormatting = defaultSelectedItemFormatting);
    
    // TODO getters and setters, mutex needed
    // The size of the menu. Each axis can be zero, but it is recommended that
    // you use expand if you do that. Essentially acts as a minimum size if
    // expand is true
    unsigned int width, height;
    
    // The position of the top left corner of the menu
    int xOffset, yOffset;
    
    // Formatting used for unselected items
    Formating formatting;
    
    // Formatting used for selected items
    Formating selectedFormatting;
    
    // If true, the menu will expand beyond its size if needed. False by
    // default
    bool expand;
    
    // If true, the offset represents the center of the menu instead.
    // False by default
    bool center;
    
    // If true, the menu will be put back in bounds if it goes out of bounds.
    // It will also be shortened if there is no way for it to fit in the
    // viewport. False by default
    bool clamp;
    
    // If true, the menu will also try to split accross columns if there is
    // not enough height for all items, instead of scrolling. False by default
    bool split; // TODO, implement splitting
    
    // Add an item to the menu
    void addItem(std::string item);
    
    // Clear all items from menu
    void clearItems();
    
    // Move cursor up or down. Tests for bounds and returns the new selection
    unsigned int moveCursor(int delta);
    
    // Draw the menu in the given viewport
    void draw(Renderer * renderer, unsigned int viewportWidth, unsigned int viewportHeight) override;
};

#endif
