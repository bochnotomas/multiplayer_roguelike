#include <thread>
#include <utility>
#include <vector>
#if defined(_WIN32) || defined(WIN32)
#include <stdio.h>
#include <conio.h>
#endif

#include "Commons.h"
#include "Renderer.h"
#include "Object.h"
#include "Map.h"
#include "Camera.h"
#include "Menu.hpp"

enum MenuItemKey {
    Normal,
    Overflowing,
    Quit,
    Filler
};

int main(int argc, char* argv[]) {
	// Main game map
	Map map;
	// generate sample map
	//map.generate_square_map(40, 40);
	map.set_preset_map();

	// sample object
	Object* player = new Object('x', Direction::NORTH, true, { 2,2 }, {Color::BLACK, Color::RED});

	// add player into main map
	map.objects.push_back(std::move(player));

	// create camera
	Camera main_cam('.', &map, { 2,2 });
    
    // Create sample menu
    Menu sampleMenu(10, 10, RENDER_WIDTH / 2, RENDER_HEIGHT / 2);
    sampleMenu.addItem(std::shared_ptr<MenuItem>(new MenuItem(MenuItemKey::Normal, "Item 1")));
    sampleMenu.addItem(std::shared_ptr<MenuItem>(new MenuItem(MenuItemKey::Normal, "Item 2")));
    sampleMenu.addItem(std::shared_ptr<MenuItem>(new MenuItem(MenuItemKey::Overflowing, "Overflowing 1")));
    sampleMenu.addItem(std::shared_ptr<MenuItem>(new MenuItem(MenuItemKey::Overflowing, "Overflowing 2")));
    sampleMenu.addItem(std::shared_ptr<MenuItem>(new MenuItem(MenuItemKey::Quit, "Quit", {Color::WHITE, Color::RED}, {Color::BLACK, Color::MAGENTA})));
    for(char c = '0'; c <= '9'; c++)
        sampleMenu.addItem(std::shared_ptr<MenuItem>(new MenuItem(MenuItemKey::Filler, std::string("Filler ") + c)));
    sampleMenu.toggleCenter(true);

	// Create a new renderer with the given dimensions
	Renderer renderer(RENDER_WIDTH, RENDER_HEIGHT);
    
    // Add camera and menu to drawables
    renderer.add_drawable(&main_cam);
    renderer.add_drawable(&sampleMenu);

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

#if defined(unix) || defined(__unix) || defined(__unix__)
		input = Renderer::getch();
#elif defined(_WIN32) || defined(WIN32)
		input = getch();
#endif
		switch (input)
		{
		case 'e':
			game_end = true;
			renderer.b_render = false;
			break;
		case 'w':
			main_cam.move(Direction::NORTH);
		break;
		case 's':
			main_cam.move(Direction::SOUTH);
		break;
		case 'a':
			main_cam.rotate(-0.1f);
		break;
		case 'd':
			main_cam.rotate(0.1f);
		break;
		case 'z':
		break;
		case 'x':
		break;
        case '9':
            sampleMenu.moveCursor(-5);
        break;
        case '3':
            sampleMenu.moveCursor(5);
        break;
        case '8':
            sampleMenu.moveCursor(-1);
        break;
        case '2':
            sampleMenu.moveCursor(1);
        break;
        case '5':
            {
                auto selected = sampleMenu.selectCursor();
                if(selected && selected->getKey() == MenuItemKey::Quit) {
                    game_end = true;
                    renderer.b_render = false;
                }
            }
            break;
		default:
			break;
		}
		player->set_position(main_cam.get_position());
		//update map objects
		map.update_objects();
		//end = std::chrono::high_resolution_clock::now();
		//dur_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
		//if (dur_ms < 1000.0f / FRAME_RATE)
		//	std::this_thread::sleep_for(std::chrono::milliseconds(1000 / FRAME_RATE - dur_ms)); // sync loop to the frame rate
	} while (!game_end);

	// end rendering
	r_thread.join();
	return 0;
}
