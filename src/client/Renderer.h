#ifndef ROGUELIKE_RENDERER_H_INCLUDED
#define ROGUELIKE_RENDERER_H_INCLUDED
#include <time.h>
#include <iostream>
#include "Formatting.hpp"
#include <thread>
#include <vector>
#if defined(unix) || defined(__unix) || defined(__unix__)
#include <unistd.h>
#include <termios.h>
#endif

class Renderer; // Forward-declare Renderer

/// A drawable class. Has a render method that takes in the target Renderer
class Drawable {
public:
    virtual void draw(Renderer* renderer) = 0;
    
    virtual ~Drawable() = default; // Virtual destructor needed since this class is abstract
};

// allows to fast writing data into console
class Renderer
{
    std::vector<Drawable*> drawables;
    std::vector<std::vector<Formating>> format_buffer;
    std::vector<std::vector<char>> chars_buffer;
    unsigned int width, height;
    
public:
	// change to false to end rendering
	bool b_render = true;
    
    // Create new Renderer with given window size
    Renderer(unsigned int viewportWidth, unsigned int viewportHeight);
    
    // Add drawable to drawables list
    void add_drawable(Drawable* drawable);
    
    // Clear drawables list
    void clear_drawables();
    
    // Draw cell to buffers
    void draw_cell(unsigned int x, unsigned int y, char character, Formating formatting);

	// prints given data into the output stream
	void render();

	std::thread spawn() {
		return std::thread(&Renderer::render, this);
	}

	// sets console window title
	void set_title(const char* title) {
	//	SetConsoleTitle(title);
		title = nullptr;
		delete title;
	}
	
	// Get width of renderer's viewport
	unsigned int getWidth();
	
	// Get height of renderer's viewport
	unsigned int getHeight();

#if defined(unix) || defined(__unix) || defined(__unix__)
	//Adapted from https://stackoverflow.com/questions/7469139/what-is-the-equivalent-to-getch-getche-in-linux
	static char getch(void) {
		char buf = 0;
		struct termios old = {0};
		fflush(stdout);
		if(tcgetattr(0, &old) < 0)
			perror("tcsetattr()");
		old.c_lflag &= ~ICANON;
		old.c_lflag &= ~ECHO;
		old.c_cc[VMIN] = 1;
		old.c_cc[VTIME] = 0;
		if(tcsetattr(0, TCSANOW, &old) < 0)
			perror("tcsetattr ICANON");
		if(read(0, &buf, 1) < 0)
			perror("read()");
		old.c_lflag |= ICANON;
		old.c_lflag |= ECHO;
		if(tcsetattr(0, TCSADRAIN, &old) < 0)
			perror("tcsetattr ~ICANON");
		//printf("%c\n", buf);
		return buf;
	}
	//End of Adapted
#endif
};

#endif
