#pragma once
#include <Windows.h> // HANDLE, console functions
#include <time.h>
#include <iostream>
#include "Commons.h"
#include <thread>
#include "Camera.h"

// allows to fast writing data into console
class Renderer
{
public:
	Renderer(Camera* cam) {
		m_console_handle = GetStdHandle(STD_OUTPUT_HANDLE);
		GetConsoleCursorInfo(m_console_handle, &m_cursor_info);
		m_cursor_info.bVisible = false;
		m_cam = cam;
	}

	// prints given data into the output stream
	void render() {
		while (true) {
			SetConsoleCursorInfo(m_console_handle, &m_cursor_info);
			SetConsoleCursorPosition(m_console_handle, { 0, 0 });
			std::cout << m_cam->get_to_render();
		}
	}

	std::thread spawn() {
		return std::thread(&Renderer::render, this);
	}

	// sets console window title
	void set_title(const char* title) {
		SetConsoleTitle(title);
		title = nullptr;
		delete title;
	}
	
private:
	HANDLE m_console_handle;
	CONSOLE_CURSOR_INFO m_cursor_info;
	Camera* m_cam;
};
