#include "Menu.hpp"

Menu::Menu(unsigned int width, unsigned int height, int xOffset, int yOffset, const Formating& formatting) :
    longestLength(0),
    selection(0),
    formatting(formatting),
    width(width),
    height(height),
    xOffset(xOffset),
    yOffset(yOffset),
    expand(false),
    center(false),
    clamp(false),
    split(false)
{}

void Menu::toggleExpand(bool toggle) {
    // Create guard for selection lock
    std::lock_guard<std::mutex> selectionGuard(selectionLock);
    
    expand = toggle;
}

void Menu::toggleCenter(bool toggle) {
    // Create guard for selection lock
    std::lock_guard<std::mutex> selectionGuard(selectionLock);
    
    center = toggle;
}

void Menu::toggleClamp(bool toggle) {
    // Create guard for selection lock
    std::lock_guard<std::mutex> selectionGuard(selectionLock);
    
    clamp = toggle;
}

void Menu::toggleSplit(bool toggle) {
    // Create guard for selection lock
    std::lock_guard<std::mutex> selectionGuard(selectionLock);
    
    split = toggle;
}

void Menu::addItem(const std::shared_ptr<MenuItem>& item) {
    // Create guard for selection lock
    std::lock_guard<std::mutex> selectionGuard(selectionLock);
    
    items.push_back(item);
    
    if(item->getLength() > longestLength)
        longestLength = item->getLength();
}

void Menu::clearItems() {
    // Create guard for selection lock
    std::lock_guard<std::mutex> selectionGuard(selectionLock);
    
    items.clear();
    longestLength = 0;
}

void Menu::moveCursor(int delta) {
    // Create guard for selection lock
    std::lock_guard<std::mutex> selectionGuard(selectionLock);
    
    if(items.empty())
        return;
    
    if(delta > 0) {
        selection += delta;
        if(selection >= items.size())
            selection = items.size() - 1;
    }
    else {
        if(-delta >= selection)
            selection = 0;
        else
            selection += delta;
    }
}

std::shared_ptr<MenuItem> Menu::selectCursor() {
    // Create guard for selection lock
    std::lock_guard<std::mutex> selectionGuard(selectionLock);
    
    // Return null if there are no menu items
    if(items.empty())
        return nullptr;
    
    return items[selection];
}

void Menu::draw(Renderer* renderer) {
    unsigned int viewportWidth = renderer->getWidth();
    unsigned int viewportHeight = renderer->getHeight();
    
    // Create guard for selection lock
    std::lock_guard<std::mutex> selectionGuard(selectionLock);
    
    // Figure out actual box size
    int actualWidth = width;
    int actualHeight = height;
    
    // Expand if needed
    if(expand) {
        if(longestLength > actualWidth)
            actualWidth = longestLength;
        
        if(items.size() > actualHeight)
            actualHeight = items.size();
    }
    
    // Clamp size if needed
    if(clamp) {
        if(actualWidth > viewportWidth)
            actualWidth = viewportWidth;
        
        if(actualHeight > viewportHeight)
            actualHeight = viewportHeight;
    }
    
    // Abort if size is 0
    if(actualWidth == 0 || actualHeight == 0)
        return;
    
    // Figure out actual box offset
    int actualXOffset = xOffset;
    int actualYOffset = yOffset;
    
    // Center if needed
    if(center) {
        actualXOffset -= actualWidth / 2;
        actualYOffset -= actualHeight / 2;
    }
    
    // Clamp if needed
    if(clamp) {
        if(actualXOffset < 0)
            actualXOffset = 0;
        else if(actualXOffset + actualWidth > viewportWidth)
            actualXOffset = viewportWidth - actualWidth;
        
        if(actualYOffset < 0)
            actualYOffset = 0;
        else if(actualYOffset + actualWidth > viewportWidth)
            actualYOffset = viewportWidth - actualWidth;
    }
    
    // TODO implement splitting
    
    // Find selection scroll
    int scroll = 0;
    if(items.size() > actualHeight) {
        scroll = selection - actualHeight / 2;
        
        // Clamp scroll
        if(scroll < 0)
            scroll = 0;
        else if(scroll > items.size() - actualHeight)
            scroll = items.size() - actualHeight;
    }
    
    // Find clipping region end and abort if out of viewport bounds
    auto itemRight = actualXOffset + actualWidth;
    auto endX = itemRight;
    if(endX <= 0)
        return;
    if(endX > viewportWidth)
        endX = viewportWidth;
    
    auto endY = actualYOffset + actualHeight;
    if(endY <= 0)
        return;
    if(endY > viewportHeight)
        endY = viewportHeight;
    
    // Draw items
    auto beginY = actualYOffset;
    if(beginY < 0)
        beginY = 0;
    for(auto y = beginY; y < endY; y++) {
        // Draw item if scroll in bounds, else, draw blank row
        int itemIndex = y - actualYOffset + scroll;
        if(itemIndex < items.size())
            items[itemIndex]->drawAt(renderer, actualXOffset, itemRight, y, itemIndex == selection);
        else {
            for(auto x = actualXOffset; x < endX; x++)
                renderer->draw_cell(x, y, ' ', formatting);
        }
    }
}
