#pragma once
#include <string>
#include <utility>
#include <vector>
#include <time.h>
#include <mutex>

#include "Map.h"
#include "Object.h"
#include "Commons.h"
#include <chrono>

class Camera final : public Object
{
public:
	Camera();
	Camera(char blank_char, Map& map, std::pair<long, long> start_position);

	// returns map in the range of camera view as a string ready to print
	std::string get_to_render();

	void move(const Direction dir);

private:
	char m_blank_char; // character to put if there is nothing to render on position
	Map m_map; // map which camera is observing
	std::mutex pos_mutex; // prevent changing position during rendering a frame
	std::vector<Object*> objects_in_range;

	// get objects in range of camera view and keep it in objects_in_range vecror
	void get_objects_in_range(std::pair<long, long> range_y, std::pair<long, long> range_x);
};
