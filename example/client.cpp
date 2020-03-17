#include <thread>
#include <utility>
#include <vector>
#if defined(_WIN32) || defined(WIN32)
#include <stdio.h>
#include <conio.h>
#endif

#include "../src/client/Renderer.h"
#include "../src/client/Camera.h"
#include "../src/client/Menu.hpp"
#include "../src/server/Object.h"
#include "../src/server/LevelGeneration2D.h"

constexpr unsigned short RENDER_WIDTH = 120;
constexpr unsigned short RENDER_HEIGHT = 30;

enum MenuItemKey {
    Normal,
    Overflowing,
    Quit,
    Filler
};

int main(int argc, char* argv[]) {
	// Main game map
	// generate sample map
	//map.generate_square_map(40, 40);
	//map.create_random_map();
    LevelGeneration2D generator;
	Map map = generator.create_random_map();

	// sample object
	std::shared_ptr<Object> player(new Object('x', Direction::NORTH, true, { 2.f,2.f }, {Color::BLACK, Color::RED}));
	//Object* object = new Object('x', Direction::NORTH, true, { 2.f,2.f }, {Color::BLACK, Color::RED});

	// add player into main map
	map.objects.push_back(player);
	//map.objects.push_back(std::move(object));

	// create camera
	std::shared_ptr<Camera> main_cam(new Camera('.', &map, { 2.f,2.f }, { 20, 10 }));
    
    // Create sample menu
    std::shared_ptr<Menu> sampleMenu(new Menu(10, 10, RENDER_WIDTH / 2, RENDER_HEIGHT / 2));
    sampleMenu->addItem(std::shared_ptr<MenuItem>(new MenuItem(MenuItemKey::Normal, "Item 1")));
    sampleMenu->addItem(std::shared_ptr<MenuItem>(new MenuItem(MenuItemKey::Normal, "Item 2")));
    sampleMenu->addItem(std::shared_ptr<MenuItem>(new MenuItem(MenuItemKey::Overflowing, "Overflowing 1")));
    sampleMenu->addItem(std::shared_ptr<MenuItem>(new MenuItem(MenuItemKey::Overflowing, "Overflowing 2")));
    sampleMenu->addItem(std::shared_ptr<MenuItem>(new MenuItem(MenuItemKey::Quit, "Quit", true, {Color::WHITE, Color::RED}, {Color::BLACK, Color::MAGENTA})));
    for(char c = '0'; c <= '9'; c++)
        sampleMenu->addItem(std::shared_ptr<MenuItem>(new MenuItem(MenuItemKey::Filler, std::string("Filler ") + c)));
    sampleMenu->toggleCenter(true);

	// Create a new renderer with the given dimensions
	Renderer renderer(RENDER_WIDTH, RENDER_HEIGHT);
    
    // Add camera and menu to drawables
    renderer.add_drawable(main_cam);
    renderer.add_drawable(sampleMenu);

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
			main_cam->move(Direction::NORTH);
		break;
		case 's':
			main_cam->move(Direction::SOUTH);
		break;
		case 'a':
			main_cam->rotate(-0.1f);
		break;
		case 'd':
			main_cam->rotate(0.1f);
		break;
		case 'z':
		break;
		case 'x':
		break;
        case '9':
            sampleMenu->moveCursor(-5);
        break;
        case '3':
            sampleMenu->moveCursor(5);
        break;
        case '8':
            sampleMenu->moveCursor(-1);
        break;
        case '2':
            sampleMenu->moveCursor(1);
        break;
        case '5':
            {
                auto selected = sampleMenu->selectCursor();
                if(selected && selected->getKey() == MenuItemKey::Quit) {
                    game_end = true;
                    renderer.b_render = false;
                }
            }
            break;
		default:
			break;
		}
		player->set_position(main_cam->get_position());
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
