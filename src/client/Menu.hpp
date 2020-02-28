#ifndef ROGUELIKE_MENU_HPP_INCLUDED
#define ROGUELIKE_MENU_HPP_INCLUDED
#include "MenuItem.hpp"
#include <memory>
#include <mutex>

// Menu class. Called by Renderer
class Menu : public Drawable {
    // The items in the menu
    std::vector<std::shared_ptr<MenuItem>> items;
    
    // Longest item length. Used for optimisation purposes
    unsigned int longestLength;
    
    // Current selection. Never >= items.size(), unless there are no items
    unsigned int selection;
    
    // Prevent changing a menu selected when a frame is being rendered
    std::mutex selectionLock;
    
    // Formatting for menu part without an item
    const Formating formatting;
    
    // The size of the menu. Each axis can be zero, but it is recommended that
    // you use expand if you do that. Essentially acts as a minimum size if
    // expand is true
    unsigned int width, height;
    
    // The position of the top left corner of the menu
    int xOffset, yOffset;
    
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
    
    // Internal function. Draws a strip of rows in a given rectangle
    void drawRows(Renderer* renderer, int left, int top, int right, int bottom, int scroll);
public:
    Menu(unsigned int width, unsigned int height, int xOffset = 0, int yOffset = 0, const Formating& formatting = {Color::NO_COLOR, Color::NO_COLOR});
    
    // Setters for private boolean properties
    void toggleExpand(bool toggle);
    void toggleCenter(bool toggle);
    void toggleClamp(bool toggle);
    void toggleSplit(bool toggle);
    
    // Add an item to the menu
    void addItem(const std::shared_ptr<MenuItem>& item);
    
    // Clear all items from menu
    void clearItems();
    
    // Move cursor up or down. Tests for bounds
    void moveCursor(int delta);
    
    // Get the current item selection. Returns the selected item or nullptr if
    // the menu has no items
    std::shared_ptr<MenuItem> selectCursor();
    
    // Draw the menu in the given viewport
    void draw(Renderer* renderer) override;
};

#endif
