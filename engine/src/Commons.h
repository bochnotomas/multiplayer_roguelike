#pragma once
#pragma warning(disable : 4996)

#include <vector>
#include <memory>
#include <string>
#include <stdexcept>
#include <stdio.h>
#include <stdlib.h>
// comment out to not allow developer functions
#define TESTING_MODE

// num of columns rendered on the screen
constexpr unsigned short RENDER_WIDTH = 100;
// num of rows rendered on the screen
constexpr unsigned short RENDER_HEIGHT = 30;
// frames per second
constexpr unsigned short FRAME_RATE = 60;

 enum class Direction {
	NORTH = 0,
	EAST,
	SOUTH,
	WEST
};

 enum class Color {
	 BLACK = 30,
	 RED,
	 GREEN,
	 YELLOW,
	 BLUE,
	 MAGENTA,
	 CYAN,
	 WHITE,
	 NO_COLOR
 };

enum ObjectType {
	BLOCK = 0,
	PLAYER,
	ENEMY
};

struct MapPoint {
	char character;
	bool accesible;
	Color color;
};

using MapPlane = std::vector<std::vector<MapPoint>>;
