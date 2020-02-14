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
	Object player = Object('x', Direction::NORTH, true, { 10,15 }, Color::MAGENTA);

	// add player into main map
	map.objects.push_back(&player);

	// create camera
	Camera* main_cam = new Camera('.', map, { 50,15 });

	// pass camera into the renderer
	Renderer renderer(main_cam);

	// set window title
	renderer.set_title("Engine Demo");

	// start rendering 
	std::thread r_thread = renderer.spawn();

	// hold times of iteration of the Main game loop
	auto begin  = std::chrono::high_resolution_clock::now(), end = std::chrono::high_resolution_clock::now();
	// hold duration between each iteration
	long long dur_ms;

	char input = ' ';

	bool game_end = false;

	// Main game loop
	do {
		begin = end;
		// input handling

		input = Renderer::getch();
		switch (input)
		{
		case 'e':
			game_end = true;
			break;
		case 'w':
			player.move(Direction::NORTH);
		break;
		case 's':
			player.move(Direction::SOUTH);
		break;
		case 'a':
			player.move(Direction::WEST);
		break;
		case 'd':
			player.move(Direction::EAST);
		break;
		default:
			break;
		}

		//update map objects
		map.update_objects();
		//end = std::chrono::high_resolution_clock::now();
		//dur_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
		//if (dur_ms < 1000.0f / FRAME_RATE)
		//	std::this_thread::sleep_for(std::chrono::milliseconds(1000 / FRAME_RATE - dur_ms)); // sync loop to the frame rate
	} while (!game_end);

	// end rendering
	renderer.b_render = false;
	r_thread.join();
	main_cam = nullptr;
	delete main_cam;
	return 0;
}