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
#include <sys/ioctl.h>
#elif defined(_WIN32) || defined(WIN32)
#include <stdio.h>
#include <conio.h>
#endif
#include <mutex>

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
    std::vector<std::shared_ptr<Drawable>> drawables;
    std::vector<std::vector<Formating>> format_buffer;
    std::vector<std::vector<char>> chars_buffer;
    unsigned int width, height;
    
public:
	// change to false to end rendering
	bool b_render = true;
    
    // Renderer lock. Create a lock guard with this mutex when changing the
    // drawables list
    std::mutex r_lock;
    
    // Create new Renderer with given window size
    Renderer(unsigned int viewportWidth, unsigned int viewportHeight);
    
    // Add drawable to drawables list
    void add_drawable(std::shared_ptr<Drawable> drawable);
    
    // Clear drawables list
    void clear_drawables();
    
    // Same as two functions above, but automatically locks mutex
    void add_drawable_lock(std::shared_ptr<Drawable> drawable);
    void clear_drawables_lock();
    
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

	//Adapted from https://stackoverflow.com/questions/7469139/what-is-the-equivalent-to-getch-getche-in-linux
	static char getch(void) {
        #if defined(unix) || defined(__unix) || defined(__unix__)
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
        #elif defined(_WIN32) || defined(WIN32)
        return _getch();
        #else
        char ch;
        std::cin >> ch;
        return ch;
        #endif
	}
	//End of Adapted
	
    // Adapted from https://www.flipcode.com/archives/_kbhit_for_Linux.shtml
    static int kbhit() {
        #if defined(unix) || defined(__unix) || defined(__unix__)
        static const int STDIN = 0;
        static bool initialized = false;

        if (! initialized) {
            // Use termios to turn off line buffering
            termios term;
            tcgetattr(STDIN, &term);
            term.c_lflag &= ~ICANON;
            tcsetattr(STDIN, TCSANOW, &term);
            setbuf(stdin, NULL);
            initialized = true;
        }

        int bytesWaiting;
        ioctl(STDIN, FIONREAD, &bytesWaiting);
        return bytesWaiting;
        #elif defined(_WIN32) || defined(WIN32)
        return _kbhit();
        #else
        return 1;
        #endif
    }
    // End of Adapted
};

#endif
