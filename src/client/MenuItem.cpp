#include "MenuItem.hpp"

MenuItem::MenuItem(int key, std::string text, bool selectable, const Formating& formatting, const Formating& selectedFormatting) :
    key(key),
    text(text),
    formatting(formatting),
    selectedFormatting(selectedFormatting),
    selectable(selectable)
{}

int MenuItem::getKey() const {
    return key;
}

unsigned int MenuItem::getLength() const {
    return text.size();
}

bool MenuItem::isSelectable() const {
    return selectable;
}

void MenuItem::drawAt(Renderer* renderer, int left, int right, int y, bool selected) const {
    // Clip y
    if(y < 0)
        return;
    const auto limitY = renderer->getHeight();
    if(y >= limitY)
        return;
    
    // Get formatting
    bool useSelectedFormatting = selected && selectable;
    const Formating& thisFormatting = useSelectedFormatting ? selectedFormatting : formatting;
    
    // Handle overflowing items
    bool overflow = false;
    int width = right - left;
    int drawableLength = text.size();
    if(drawableLength > width) {
        drawableLength = width - 3;
        if(drawableLength <= 0)
            drawableLength = 1;
        
        overflow = true;
    }
    
    // Draw item characters
    const auto limitX = renderer->getWidth();
    for(auto c = 0; c < drawableLength; c++) {
        // Clip x
        auto charX = left + c;
        if(charX >= limitX)
            break;
        if(charX < 0)
            continue;
        
        // Draw
        renderer->draw_cell(charX, y, text[c], thisFormatting);
    }
    
    // Draw overflow and blank spaces if needed
    const auto itemEndX = left + drawableLength;
    const auto overflowX = itemEndX + 3;
    for(auto x = itemEndX; x < right; x++) {
        // Clip x
        if(x < 0)
            continue;
        if(x >= limitX)
            break;
        
        // Draw
        if(overflow && x < overflowX)
            renderer->draw_cell(x, y, '.', thisFormatting);
        else
            renderer->draw_cell(x, y, ' ', thisFormatting);
    }
}

bool MenuItem::onInput(char input) {
    (void)input;
    return false;
}
