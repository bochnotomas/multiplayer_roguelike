#include <thread>
#include <utility>
#include <vector>

#include "Commons.h"
#include "Renderer.h"
#include "Object.h"
#include "Map.h"
#include "Camera.h"

int main(int argc, char* argv[]) {
	// Main game map
	Map map;
	// generate sample map
	map.generate_square_map(100, 30);

	// sample object
	Object player = Object('x', Direction::NORTH, true, { 10,15 });

	// add player into main map
	map.objects.push_back(&player);

	// create camera
	Camera* main_cam = new Camera(' ', map, { 50,15 });

	// pass camera into the renderer
	Renderer renderer(main_cam);

	// set window title
	renderer.set_title("Engine Demo");

	// start rendering 
	std::thread r_thread = renderer.spawn();

	// hold times of iteration of the Main game loop
	std::chrono::steady_clock::time_point begin, end;
	// hold duration between each iteration
	long long dur_ms;
	
	begin = end = std::chrono::high_resolution_clock::now();

	// Main game loop
	do {
		begin = end;
		// input handling
		if (GetAsyncKeyState(static_cast<unsigned short>('W')) & 0x8000) {
			player.move(Direction::NORTH);
		}
		if (GetAsyncKeyState(static_cast<unsigned short>('S')) & 0x8000) {
			player.move(Direction::SOUTH);
		}
		if (GetAsyncKeyState(static_cast<unsigned short>('A')) & 0x8000) {
			player.move(Direction::WEST);
		}
		if (GetAsyncKeyState(static_cast<unsigned short>('D')) & 0x8000) {
			player.move(Direction::EAST);
		}
		if (GetAsyncKeyState(static_cast<unsigned short>('E')) & 0x8000) {
			break;
		}
		//update map objects
		map.update_objects();
		end = std::chrono::high_resolution_clock::now();
		dur_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
		if (dur_ms < 1000.0f / FRAME_RATE)
			Sleep(1000.0f / FRAME_RATE - dur_ms); // sync loop to the frame rate
	} while (true);

	// end rendering
	r_thread.detach();
	main_cam = nullptr;
	delete main_cam;
	return 0;
}