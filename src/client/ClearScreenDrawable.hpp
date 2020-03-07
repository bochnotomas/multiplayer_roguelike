#ifndef ROGUELIKE_CLEAR_SCREEN_DRAWABLE_HPP_INCLUDED
#define ROGUELIKE_CLEAR_SCREEN_DRAWABLE_HPP_INCLUDED
#include "Renderer.h"

/// A drawable that clears the screen when drawn
class ClearScreenDrawable : public Drawable {
    char character;
    Formating formatting;
public:
    /// Create new screen clearer. formatting is the cell formatting that is
    /// used for clearing a cell. Same applies for character, but its the cell
    /// character to replace instead
    ClearScreenDrawable(char character = ' ', Formating formatting = { Color::NO_COLOR, Color::NO_COLOR });
    
    void draw(Renderer * renderer) override;
};

#endif
