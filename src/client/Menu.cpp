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

void Menu::setSplit(unsigned int minColumnWidth) {
    // Create guard for selection lock
    std::lock_guard<std::mutex> selectionGuard(selectionLock);
    
    split = minColumnWidth;
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

void Menu::drawRows(Renderer* renderer, int left, int top, int right, int bottom, int scroll) {
    const unsigned int viewportWidth = renderer->getWidth();
    const unsigned int viewportHeight = renderer->getHeight();
    
    // Find clipping region end and abort if out of viewport bounds
    auto endX = right;
    if(endX <= 0)
        return;
    if(endX > viewportWidth)
        endX = viewportWidth;
    
    auto endY = bottom;
    if(endY <= 0)
        return;
    if(endY > viewportHeight)
        endY = viewportHeight;
    
    // Draw items
    auto beginX = left;
    if(beginX < 0)
        beginX = 0;
    
    auto beginY = top;
    if(beginY < 0)
        beginY = 0;
    
    for(auto y = beginY; y < endY; y++) {
        // Draw item if scroll in bounds, else, draw blank row
        int itemIndex = y - top + scroll;
        if(itemIndex < items.size())
            items[itemIndex]->drawAt(renderer, left, right, y, itemIndex == selection);
        else {
            for(auto x = beginX; x < endX; x++)
                renderer->draw_cell(x, y, ' ', formatting);
        }
    }
}

void Menu::draw(Renderer* renderer) {
    const unsigned int viewportWidth = renderer->getWidth();
    const unsigned int viewportHeight = renderer->getHeight();
    
    // Create guard for selection lock
    std::lock_guard<std::mutex> selectionGuard(selectionLock);
    
    // Figure out actual box size
    int actualWidth = width;
    int actualHeight = height;
    int colCount;
    
    // Count columns if splitting
    if(split) {
        colCount = items.size() / actualHeight;
        
        // Correct column count for empty menus and partially filled
        // columns
        if(items.empty() || items.size() % actualHeight)
            colCount++;
    }
    
    // Expand if needed
    if(expand) {
        if(split) {
            // If split is enabled, only expand horizontally
            int neededWidth;
            if(split > longestLength)
                neededWidth = colCount * split;
            else
                neededWidth = colCount * longestLength;
            
            if(neededWidth > actualWidth)
                actualWidth = neededWidth;
        }
        else {
            // ... else, expand on both axis
            if(longestLength > actualWidth)
                actualWidth = longestLength;
            
            if(items.size() > actualHeight)
                actualHeight = items.size();
        }
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
        else if(actualYOffset + actualHeight > viewportHeight)
            actualYOffset = viewportHeight - actualHeight;
    }
    
    // Draw menu items
    if(split) {
        // Splitting, multiple columns
        int colWidth = actualWidth / colCount;
        if(colWidth < split)
            colWidth = split;
        
        // Find selection scroll
        int scroll = 0;
        int screenCols = actualWidth / colWidth;
        if(screenCols > colCount)
            screenCols = colCount;
        
        if(colCount * actualHeight > items.size()) {
            scroll = selection / actualHeight - screenCols / 2;
            
            // Clamp scroll
            if(scroll < 0)
                scroll = 0;
            else if(scroll > colCount - screenCols)
                scroll = colCount - screenCols;
        }
        
        // Draw columns
        int rightLimit = actualXOffset + actualWidth;
        for(int c = 0; c < screenCols; c++) {
            // Stop if left is out of bounds
            int left = actualXOffset + c * colWidth;
            if(left >= rightLimit)
                break;
            
            // Shrink last drawable column if needed
            int right = left + colWidth;
            if(right > rightLimit)
                right = rightLimit;
            
            // Draw rows in this column
            drawRows(renderer, left, actualYOffset, right, actualYOffset + actualHeight, (c + scroll) * actualHeight);
        }
        
        // Dead space if needed
        int lastLeft = actualXOffset + screenCols * colWidth;
        if(lastLeft != rightLimit)
            drawRows(renderer, lastLeft, actualYOffset, rightLimit, actualYOffset + actualHeight, (screenCols + scroll) * actualHeight);
    }
    else {
        // No splitting, single column
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
        
        // Draw
        drawRows(renderer, actualXOffset, actualYOffset, actualXOffset + actualWidth, actualYOffset + actualHeight, scroll);
    }
}
