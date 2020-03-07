#include "ClearScreenDrawable.hpp"

ClearScreenDrawable::ClearScreenDrawable(char character, Formating formatting) :
    character(character),
    formatting(formatting)
{}

void ClearScreenDrawable::draw(Renderer* renderer) {
    // Clear screen with no formatting
    const auto width = renderer->getWidth();
    const auto height = renderer->getHeight();
    for(auto y = 0; y < height; y++) {
        for(auto x = 0; x < width; x++)
            renderer->draw_cell(x, y, character, formatting);
    }
}

