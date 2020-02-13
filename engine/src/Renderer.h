#pragma once
#include <time.h>
#include <iostream>
#include "Commons.h"
#include <thread>
#include "Camera.h"
#include <unistd.h>
#include <termios.h>

// allows to fast writing data into console
class Renderer
{
public:
	// change to false to end rendering
	bool b_render = true;

	Renderer(Camera* cam) : m_cam(cam) {
	}

	// prints given data into the output stream
	void render() {
		std::system("clear");
		// go to new line and disable cursor
		std::cout << "\e[?25l";
		while (b_render) {
			// go to (0,0) position
			std::cout << "\033[0;0f" << 
					  m_cam->get_to_render();// << "W - move up, S - move down, A - move left, D move right, E - exit";
		}
		// re-enable cursor
		std::cout << "\033[0;0f\e[?25h";
		// clear console
		std::system("clear");
	}

	std::thread spawn() {
		return std::thread(&Renderer::render, this);
	}

	// sets console window title
	void set_title(const char* title) {
	//	SetConsoleTitle(title);
		title = nullptr;
		delete title;
	}

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
	
private:
	Camera* m_cam;
};
