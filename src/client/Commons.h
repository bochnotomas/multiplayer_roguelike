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
constexpr unsigned short RENDER_WIDTH = 120;
// num of rows rendered on the screen
constexpr unsigned short RENDER_HEIGHT = 30;
// num of columns for minimap
constexpr unsigned short MINIMAP_WIDTH = 20;
// num of rows for minimap
constexpr unsigned short MINIMAP_HEIGHT = 10;
// depth of 2.5D rendering
constexpr float RENDER_DEPTH = 16.0f;

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

struct Formating {
	Color text_color;
	Color background_color;
	// TODO: add bold, curved etc...
};

struct MapPoint {
	char character;
	bool accesible;
	Formating formating;
};

using MapPlane = std::vector<std::vector<MapPoint>>;
