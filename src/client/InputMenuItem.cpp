#include "InputMenuItem.hpp"
#include <chrono>

InputMenuItem::InputMenuItem(int key, std::string text, std::string defaultValue, const Formating& formatting, const Formating& selectedFormatting) :
    MenuItem(key, text, true, formatting, selectedFormatting),
    value(defaultValue),
    locked(false)
{}

std::string InputMenuItem::get() const {
    return value;
}

void InputMenuItem::set(std::string newValue) {
    value = newValue;
}

bool InputMenuItem::onInput(char input) {
    if(locked) {
        if(input == '\x1b' || input == '\n' || input == '\r') // Escape/Enter
            locked = false;
        else if(input == '\x7f' || input == '\x08') { // Delete/Backspace
            if(!value.empty())
                value.pop_back();
        }
        else if(input >= ' ' && input <= '~')
            value += input;
    }
    else {
        if(input == ' ' || input == '\n' || input == '\r')
            locked = true;
        else
            return false;
    }
    
    return true;
}

void InputMenuItem::unlock() {
    locked = false;
}

unsigned int InputMenuItem::getLength() const {
    return text.size() + value.size() + 2;
}

void InputMenuItem::drawAt(Renderer* renderer, int left, int right, int y, bool selected) const {
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
    if(locked)
        width--;
    
    std::string fullText = locked ? value : (text + ": " + value);
    int drawableLength = fullText.size();
    if(drawableLength > width) {
        if(!locked) {
            drawableLength = width - 3;
            if(drawableLength <= 0)
                drawableLength = 1;
        }
        else {
            drawableLength = width;
            fullText = fullText.substr(fullText.size() - width, drawableLength);
        }
        
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
        renderer->draw_cell(charX, y, fullText[c], thisFormatting);
    }
    
    // Draw overflow and blank spaces if needed
    const auto itemEndX = left + drawableLength;
    const auto overflowX = itemEndX + 3;
    
    bool blink = false;
    if(locked) {
        auto now = std::chrono::high_resolution_clock::now();
        auto epochDuration = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch());
        if((epochDuration.count() / 300) % 2)
            blink = true;
    }
    
    for(auto x = itemEndX; x < right; x++) {
        // Clip x
        if(x < 0)
            continue;
        if(x >= limitX)
            break;
        
        // Draw
        if(locked && ((overflow && x == right - 1) || (!overflow && x == itemEndX))) {
            if(blink)
                renderer->draw_cell(x, y, ' ', formatting);
            else
                renderer->draw_cell(x, y, ' ', selectedFormatting);
        }
        else if(!locked && overflow && x < overflowX)
            renderer->draw_cell(x, y, '.', thisFormatting);
        else
            renderer->draw_cell(x, y, ' ', thisFormatting);
    }
}
