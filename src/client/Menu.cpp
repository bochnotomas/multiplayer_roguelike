#include "Menu.hpp"

Menu::Menu(unsigned int width, unsigned int height, int xOffset, int yOffset, Formating formatting, Formating selectedFormatting) :
    longestLength(0),
    selection(0),
    width(width),
    height(height),
    xOffset(xOffset),
    yOffset(yOffset),
    formatting(formatting),
    selectedFormatting(selectedFormatting),
    expand(false),
    center(false),
    clamp(false),
    split(false)
{}

void Menu::addItem(std::string item) {
    items.push_back(item);
    
    if(item.size() > longestLength)
        longestLength = item.size();
}

void Menu::clearItems() {
    items.clear();
    longestLength = 0;
}

unsigned int Menu::moveCursor(int delta) {
    // Create guard for selection lock
    std::lock_guard<std::mutex> selectionGuard(selectionLock);
    
    if(items.empty())
        return selection = 0;
    
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
    
    return selection;
}

void Menu::draw(Renderer* renderer, unsigned int viewportWidth, unsigned int viewportHeight) {
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
        scroll = selection - actualHeight + 2;
        if(scroll < 0)
            scroll = 0;
        else if(selection == items.size() - 1)
            scroll--;
    }
    
    // Find clipping region end and abort if out of viewport bounds
    auto endX = actualXOffset + actualWidth;
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
    for(auto i = 0; i < items.size(); i++) {
        // Abort when out of bounds
        const auto thisY = actualYOffset + i;
        if(thisY >= endY)
            break;
        if(thisY < 0)
            continue;
        
        // Handle selection
        const auto thisSelection = i + scroll;
        Formating& currentFormatting = (thisSelection == selection) ? selectedFormatting : formatting;
        const auto& thisItem = items[thisSelection];
        
        // Handle overflowing items
        bool overflow = false;
        int drawableLength = thisItem.size();
        if(drawableLength > actualWidth) {
            drawableLength = actualWidth - 3;
            if(drawableLength <= 0)
                drawableLength = 1;
            
            overflow = true;
        }
        
        // Draw item characters
        for(auto c = 0; c < drawableLength; c++) {
            // Clip
            auto charX = actualXOffset + c;
            if(charX >= endX)
                break;
            if(charX < 0)
                continue;
            
            // Draw
            renderer->draw_cell(charX, thisY, thisItem[c], currentFormatting);
        }
        
        // Draw overflow and blank spaces if needed
        const auto itemEndX = actualXOffset + drawableLength;
        const auto overflowX = itemEndX + 3;
        for(auto x = itemEndX; x < endX; x++) {
            // Clip
            if(x >= endX)
                break;
            if(x < 0)
                continue;
            
            // Draw
            if(overflow && x < overflowX)
                renderer->draw_cell(x, thisY, '.', currentFormatting);
            else
                renderer->draw_cell(x, thisY, ' ', currentFormatting);
        }
    }
    
    // Draw blank rows if needed
    for(auto y = items.size() + actualYOffset - scroll; y < endY; y++) {
        for(auto x = actualXOffset; x < endX; x++)
            renderer->draw_cell(x, y, ' ', formatting);
    }
}
