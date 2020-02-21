#include "Renderer.h"

Renderer::Renderer(unsigned int viewportWidth, unsigned int viewportHeight) :
    width(viewportWidth),
    height(viewportHeight)
{
    // Default formatting
    const Formating defaultFormatting {
        Color::NO_COLOR,
        Color::NO_COLOR
    };
    
    // Create buffers
    format_buffer.reserve(viewportHeight);
    chars_buffer.reserve(viewportHeight);
    std::vector<Formating> format_row(viewportWidth, defaultFormatting);
    std::vector<char> chars_row(viewportWidth, ' ');
    for(unsigned int y = 0; y < viewportHeight; y++) {
        format_buffer.push_back(format_row);
        chars_buffer.push_back(chars_row);
    }
}

void Renderer::add_drawable(Drawable* drawable) {
    drawables.push_back(drawable);
}

void Renderer::draw_cell(unsigned int x, unsigned int y, char character, Formating formatting) {
    if(y >= height || x >= width)
        throw std::range_error("Renderer::draw_cell called at position " + std::to_string(x) + ", " + std::to_string(y) + " but size is " + std::to_string(width) + ", " + std::to_string(height));
    
    format_buffer[y][x] = formatting;
    chars_buffer[y][x] = character;
}
    
void Renderer::render() {
    // clear the screen
    std::cout << "\033[2J\033[H";
    // go to new line and disable cursor
    std::cout << "\033[?25l";
    
    while (b_render) {
        // go to (0,0) position
        std::cout << "\033[0;0f";
        
        // Draw all drawables
        for(auto drawable : drawables)
            drawable->draw(this, width, height);
        
        // Turn buffer into lines of formatted text
        std::string string_buffer;
        string_buffer.reserve(width * height * 2); // Reserve twice as much minimum space needed
        Color last_back_color = Color::NO_COLOR;
        Color last_text_color = Color::NO_COLOR;
        for(unsigned int y = 0; y < height; y++) {
            for(unsigned int x = 0; x < width; x++) {
                // Add formatting if different
                const Formating& formatting = format_buffer[y][x];
                if(formatting.background_color != last_back_color) {
                    last_back_color = formatting.background_color;
                    if(last_back_color == Color::NO_COLOR)
                        string_buffer += "\033[49m"; // Reset background only
                    else
                        string_buffer += "\033[" + std::to_string(static_cast<int>(last_back_color) + 10) + "m"; // Set background
                }
                if(formatting.text_color != last_text_color) {
                    last_text_color = formatting.text_color;
                    if(last_text_color == Color::NO_COLOR)
                        string_buffer += "\033[39m"; // Reset text color only
                    else
                        string_buffer += "\033[" + std::to_string(static_cast<int>(last_text_color)) + "m"; // Set text color
                }
                
                // Add cell character
                string_buffer += chars_buffer[y][x];
            }
            
            // Newline
            string_buffer += '\n';
        }
        
        // Print buffer and reset colors
        std::cout << string_buffer << "\033[0m" << std::flush;
    }
    
    std::cout << "\033[0m";
    // re-enable cursor
    std::cout << "\033[2J\033[H";
    // clear console
}
