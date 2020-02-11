#pragma once
#include <string>
#include <utility>
#include <vector>
#include <time.h>

#include "Map.h"
#include "Object.h"
#include "Commons.h"
#include <chrono>

class Camera final : public Object
{
public:
	Camera();
	Camera(char blank_char, Map& map, std::pair<long, long> start_position);

	std::string get_to_render();

private:
	char m_blank_char; // character to put if there is nothing to render on position
	Map m_map; // map which camera is observing


};
