#pragma once
#pragma warning(disable : 4996)

#include <vector>
#include <memory>
#include <string>
#include <stdexcept>

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
	 BLACK = 0,
	 BLUE,
	 GREEN,
	 AQUA,
	 RED,
	 PURPLE,
	 YELLOW,
	 WHITE,
	 GRAY,
	 LIGHT_BLUE,
	 LIGHT_GREEN,
	 LIGHT_AQUA,
	 LIGHT_RED,
	 LIGHT_PURPLE,
	 LIGHT_YELLOW,
	 BRIGHT_WHITE,
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

// TODO: Remove when C++20 will come by std::format
// Adapted from https://stackoverflow.com/questions/2342162/stdstring-formatting-like-sprintf
template<typename ... Args>
std::string string_format(const std::string& format, Args ... args)
{
	size_t size = _snprintf(nullptr, 0, format.c_str(), args ...) + 1; // Extra space for '\0'
	if (size <= 0) { throw std::runtime_error("Error during formatting."); }
	std::unique_ptr<char[]> buf(new char[size]);
	_snprintf(buf.get(), size, format.c_str(), args ...);
	return std::string(buf.get(), buf.get() + size - 1); // We don't want the '\0' inside
}
// end of adapted

using MapPlane = std::vector<std::vector<MapPoint>>;
