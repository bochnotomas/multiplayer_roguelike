#ifndef ROGUELIKE_MAP_H_INCLUDED
#define ROGUELIKE_MAP_H_INCLUDED
#include <vector>
#include <utility>
#include "LevelGeneration2D.h"
#include "Object.h"
#include "../client/Formatting.hpp"

struct MapPoint {
	char character;
	bool accesible;
	Formating formating;
};

using MapPlane = std::vector<std::vector<MapPoint>>;

class Map
{
public:
	Map();

	~Map();

	// holds objects related with map
	std::vector<Object*> objects;

	// updates each object related with the map
	void update_objects();

	// returns map plane
	MapPlane* get_map_plane();

	// returns size of the map
	std::pair<unsigned long, unsigned long> get_map_size();

#ifndef DEVMODE
	// generate square map
	void generate_square_map(unsigned int width, unsigned int height) {
		m_size = { width, height };
		m_plane = MapPlane(height, std::vector<MapPoint>(width, { ' ', true, {Color::BLACK, Color::GREEN} }));
		m_plane[0] = std::vector<MapPoint>(width, { '#', false, {Color::WHITE, Color::MAGENTA} });
		m_plane[height-1] = std::vector<MapPoint>(width, { '#', false, {Color::WHITE, Color::MAGENTA} });

		for (size_t i = 1; i < height; i++) {
			m_plane[i][0] = { '#', false, {Color::WHITE, Color::MAGENTA} };
			m_plane[i][width-1] = { '#', false, {Color::WHITE, Color::MAGENTA} };
		}

		int x=0;
	}

	void create_random_map() {
		LevelGeneration2D one;

		one.setGrid();

		for (int i = 0; i <= 100; i++) {
			one.generation();
		}

		for (int i = 0; i <= 3; i++) {
			one.refine();
		}
		//end of levelgeneration2D code

		m_size = { 100, 100 };
		m_plane = MapPlane(100, std::vector<MapPoint>(100, { ' ', true, {Color::BLACK, Color::GREEN} }));

		std::vector<std::vector<char>> grid = one.getGrid();

		for (int y = 0; y <= 99; y++) {
			for (int x = 0; x <= 99; x++) {
				if (grid[y][x] == ' ') {
					m_plane[x][y] = { ' ', true, {Color::BLACK, Color::BLACK} };
				}
				else if (grid[y][x] == '#') {
					m_plane[x][y] = { '#', false, {Color::WHITE, Color::GREEN} };
				}
			}
		}
	}
	void set_preset_map(){
		m_size = {16, 16};
		m_plane = MapPlane(16, std::vector<MapPoint>(16, { ' ', true, {Color::BLACK, Color::GREEN} }));
		m_plane[0] =  {{'#', false, {Color::WHITE, Color::GREEN} },{'#', false, {Color::WHITE, Color::GREEN} },{'#', false, {Color::WHITE, Color::GREEN} },{'#', false, {Color::WHITE, Color::GREEN} },{'#', false, {Color::WHITE, Color::GREEN} },{'#', false, {Color::WHITE, Color::GREEN} },{'#', false, {Color::WHITE, Color::GREEN} },{'#', false, {Color::WHITE, Color::GREEN} },{' ', true, {Color::BLACK, Color::BLACK} },{' ', true, {Color::BLACK, Color::BLACK} },{' ', true, {Color::BLACK, Color::BLACK} },{' ', true, {Color::BLACK, Color::BLACK} },{' ', true, {Color::BLACK, Color::BLACK} },{' ', true, {Color::BLACK, Color::BLACK} },{' ', true, {Color::BLACK, Color::BLACK} },{' ', true, {Color::BLACK, Color::BLACK} }};
		m_plane[1] =  {{'#', false, {Color::WHITE, Color::GREEN} },{' ', true, {Color::BLACK, Color::BLACK} },{' ', true, {Color::BLACK, Color::BLACK} },{' ', true, {Color::BLACK, Color::BLACK} },{' ', true, {Color::BLACK, Color::BLACK} },{' ', true, {Color::BLACK, Color::BLACK} },{' ', true, {Color::BLACK, Color::BLACK} },{' ', true, {Color::BLACK, Color::BLACK} },{' ', true, {Color::BLACK, Color::BLACK} },{' ', true, {Color::BLACK, Color::BLACK} },{' ', true, {Color::BLACK, Color::BLACK} },{' ', true, {Color::BLACK, Color::BLACK} },{' ', true, {Color::BLACK, Color::BLACK} },{' ', true, {Color::BLACK, Color::BLACK} },{' ', true, {Color::BLACK, Color::BLACK} },{' ', true, {Color::BLACK, Color::BLACK} }};
		m_plane[2] =  {{'#', false, {Color::WHITE, Color::GREEN} },{' ', true, {Color::BLACK, Color::BLACK} },{' ', true, {Color::BLACK, Color::BLACK} },{' ', true, {Color::BLACK, Color::BLACK} },{' ', true, {Color::BLACK, Color::BLACK} },{' ', true, {Color::BLACK, Color::BLACK} },{' ', true, {Color::BLACK, Color::BLACK} },{' ', true, {Color::BLACK, Color::BLACK} },{'#', false, {Color::WHITE, Color::GREEN} },{'#', false, {Color::WHITE, Color::GREEN} },{'#', false, {Color::WHITE, Color::GREEN} },{'#', false, {Color::WHITE, Color::GREEN} },{'#', false, {Color::WHITE, Color::GREEN} },{'#', false, {Color::WHITE, Color::GREEN} },{'#', false, {Color::WHITE, Color::GREEN} },{'#', false, {Color::WHITE, Color::GREEN} }};
		m_plane[3] =  {{'#', false, {Color::WHITE, Color::GREEN} },{' ', true, {Color::BLACK, Color::BLACK} },{' ', true, {Color::BLACK, Color::BLACK} },{' ', true, {Color::BLACK, Color::BLACK} },{' ', true, {Color::BLACK, Color::BLACK} },{' ', true, {Color::BLACK, Color::BLACK} },{' ', true, {Color::BLACK, Color::BLACK} },{' ', true, {Color::BLACK, Color::BLACK} },{' ', true, {Color::BLACK, Color::BLACK} },{' ', true, {Color::BLACK, Color::BLACK} },{' ', true, {Color::BLACK, Color::BLACK} },{' ', true, {Color::BLACK, Color::BLACK} },{' ', true, {Color::BLACK, Color::BLACK} },{' ', true, {Color::BLACK, Color::BLACK} },{' ', true, {Color::BLACK, Color::BLACK} },{'#', false, {Color::WHITE, Color::GREEN} }};
		m_plane[4] =  {{'#', false, {Color::WHITE, Color::GREEN} },{' ', true, {Color::BLACK, Color::BLACK} },{' ', true, {Color::BLACK, Color::BLACK} },{' ', true, {Color::BLACK, Color::BLACK} },{' ', true, {Color::BLACK, Color::BLACK} },{' ', true, {Color::BLACK, Color::BLACK} },{' ', true, {Color::BLACK, Color::BLACK} },{'#', false, {Color::WHITE, Color::GREEN} },{'#', false, {Color::WHITE, Color::GREEN} },{' ', true, {Color::BLACK, Color::BLACK} },{' ', true, {Color::BLACK, Color::BLACK} },{' ', true, {Color::BLACK, Color::BLACK} },{' ', true, {Color::BLACK, Color::BLACK} },{' ', true, {Color::BLACK, Color::BLACK} },{' ', true, {Color::BLACK, Color::BLACK} },{'#', false, {Color::WHITE, Color::GREEN} }};
		m_plane[5] =  {{'#', false, {Color::WHITE, Color::GREEN} },{' ', true, {Color::BLACK, Color::BLACK} },{' ', true, {Color::BLACK, Color::BLACK} },{' ', true, {Color::BLACK, Color::BLACK} },{' ', true, {Color::BLACK, Color::BLACK} },{' ', true, {Color::BLACK, Color::BLACK} },{' ', true, {Color::BLACK, Color::BLACK} },{'#', false, {Color::WHITE, Color::GREEN} },{'#', false, {Color::WHITE, Color::GREEN} },{' ', true, {Color::BLACK, Color::BLACK} },{' ', true, {Color::BLACK, Color::BLACK} },{' ', true, {Color::BLACK, Color::BLACK} },{' ', true, {Color::BLACK, Color::BLACK} },{' ', true, {Color::BLACK, Color::BLACK} },{' ', true, {Color::BLACK, Color::BLACK} },{'#', false, {Color::WHITE, Color::GREEN} }};
		m_plane[6] =  {{'#', false, {Color::WHITE, Color::GREEN} },{' ', true, {Color::BLACK, Color::BLACK} },{' ', true, {Color::BLACK, Color::BLACK} },{' ', true, {Color::BLACK, Color::BLACK} },{' ', true, {Color::BLACK, Color::BLACK} },{' ', true, {Color::BLACK, Color::BLACK} },{' ', true, {Color::BLACK, Color::BLACK} },{' ', true, {Color::BLACK, Color::BLACK} },{' ', true, {Color::BLACK, Color::BLACK} },{' ', true, {Color::BLACK, Color::BLACK} },{' ', true, {Color::BLACK, Color::BLACK} },{' ', true, {Color::BLACK, Color::BLACK} },{' ', true, {Color::BLACK, Color::BLACK} },{' ', true, {Color::BLACK, Color::BLACK} },{' ', true, {Color::BLACK, Color::BLACK} },{'#', false, {Color::WHITE, Color::GREEN} }};
		m_plane[7] =  {{'#', false, {Color::WHITE, Color::GREEN} },{'#', false, {Color::WHITE, Color::GREEN} },{'#', false, {Color::WHITE, Color::GREEN} },{' ', true, {Color::BLACK, Color::BLACK} },{' ', true, {Color::BLACK, Color::BLACK} },{' ', true, {Color::BLACK, Color::BLACK} },{' ', true, {Color::BLACK, Color::BLACK} },{' ', true, {Color::BLACK, Color::BLACK} },{' ', true, {Color::BLACK, Color::BLACK} },{' ', true, {Color::BLACK, Color::BLACK} },{' ', true, {Color::BLACK, Color::BLACK} },{' ', true, {Color::BLACK, Color::BLACK} },{' ', true, {Color::BLACK, Color::BLACK} },{' ', true, {Color::BLACK, Color::BLACK} },{' ', true, {Color::BLACK, Color::BLACK} },{'#', false, {Color::WHITE, Color::GREEN} }};
		m_plane[8] =  {{'#', false, {Color::WHITE, Color::GREEN} },{'#', false, {Color::WHITE, Color::GREEN} },{' ', true, {Color::BLACK, Color::BLACK} },{' ', true, {Color::BLACK, Color::BLACK} },{' ', true, {Color::BLACK, Color::BLACK} },{' ', true, {Color::BLACK, Color::BLACK} },{' ', true, {Color::BLACK, Color::BLACK} },{' ', true, {Color::BLACK, Color::BLACK} },{' ', true, {Color::BLACK, Color::BLACK} },{' ', true, {Color::BLACK, Color::BLACK} },{' ', true, {Color::BLACK, Color::BLACK} },{' ', true, {Color::BLACK, Color::BLACK} },{' ', true, {Color::BLACK, Color::BLACK} },{' ', true, {Color::BLACK, Color::BLACK} },{' ', true, {Color::BLACK, Color::BLACK} },{'#', false, {Color::WHITE, Color::GREEN} }};
		m_plane[9] =  {{'#', false, {Color::WHITE, Color::GREEN} },{' ', true, {Color::BLACK, Color::BLACK} },{' ', true, {Color::BLACK, Color::BLACK} },{' ', true, {Color::BLACK, Color::BLACK} },{' ', true, {Color::BLACK, Color::BLACK} },{' ', true, {Color::BLACK, Color::BLACK} },{' ', true, {Color::BLACK, Color::BLACK} },{' ', true, {Color::BLACK, Color::BLACK} },{'#', false, {Color::WHITE, Color::GREEN} },{'#', false, {Color::WHITE, Color::GREEN} },{'#', false, {Color::WHITE, Color::GREEN} },{'#', false, {Color::WHITE, Color::GREEN} },{' ', true, {Color::BLACK, Color::BLACK} },{'#', false, {Color::WHITE, Color::GREEN} },{'#', false, {Color::WHITE, Color::GREEN} },{'#', false, {Color::WHITE, Color::GREEN} }};
		m_plane[10] = {{'#', false, {Color::WHITE, Color::GREEN} },{' ', true, {Color::BLACK, Color::BLACK} },{' ', true, {Color::BLACK, Color::BLACK} },{' ', true, {Color::BLACK, Color::BLACK} },{' ', true, {Color::BLACK, Color::BLACK} },{' ', true, {Color::BLACK, Color::BLACK} },{' ', true, {Color::BLACK, Color::BLACK} },{' ', true, {Color::BLACK, Color::BLACK} },{'#', false, {Color::WHITE, Color::GREEN} },{' ', true, {Color::BLACK, Color::BLACK} },{' ', true, {Color::BLACK, Color::BLACK} },{' ', true, {Color::BLACK, Color::BLACK} },{' ', true, {Color::BLACK, Color::BLACK} },{' ', true, {Color::BLACK, Color::BLACK} },{' ', true, {Color::BLACK, Color::BLACK} },{'#', false, {Color::WHITE, Color::GREEN} }};
		m_plane[11] = {{'#', false, {Color::WHITE, Color::GREEN} },{' ', true, {Color::BLACK, Color::BLACK} },{' ', true, {Color::BLACK, Color::BLACK} },{' ', true, {Color::BLACK, Color::BLACK} },{' ', true, {Color::BLACK, Color::BLACK} },{' ', true, {Color::BLACK, Color::BLACK} },{' ', true, {Color::BLACK, Color::BLACK} },{' ', true, {Color::BLACK, Color::BLACK} },{'#', false, {Color::WHITE, Color::GREEN} },{' ', true, {Color::BLACK, Color::BLACK} },{' ', true, {Color::BLACK, Color::BLACK} },{' ', true, {Color::BLACK, Color::BLACK} },{' ', true, {Color::BLACK, Color::BLACK} },{' ', true, {Color::BLACK, Color::BLACK} },{' ', true, {Color::BLACK, Color::BLACK} },{'#', false, {Color::WHITE, Color::GREEN} }};
		m_plane[12] = {{'#', false, {Color::WHITE, Color::GREEN} },{' ', true, {Color::BLACK, Color::BLACK} },{' ', true, {Color::BLACK, Color::BLACK} },{' ', true, {Color::BLACK, Color::BLACK} },{' ', true, {Color::BLACK, Color::BLACK} },{' ', true, {Color::BLACK, Color::BLACK} },{' ', true, {Color::BLACK, Color::BLACK} },{' ', true, {Color::BLACK, Color::BLACK} },{' ', true, {Color::BLACK, Color::BLACK} },{' ', true, {Color::BLACK, Color::BLACK} },{' ', true, {Color::BLACK, Color::BLACK} },{' ', true, {Color::BLACK, Color::BLACK} },{' ', true, {Color::BLACK, Color::BLACK} },{' ', true, {Color::BLACK, Color::BLACK} },{' ', true, {Color::BLACK, Color::BLACK} },{'#', false, {Color::WHITE, Color::GREEN} }};
		m_plane[13] = {{'#', false, {Color::WHITE, Color::GREEN} },{' ', true, {Color::BLACK, Color::BLACK} },{' ', true, {Color::BLACK, Color::BLACK} },{' ', true, {Color::BLACK, Color::BLACK} },{' ', true, {Color::BLACK, Color::BLACK} },{' ', true, {Color::BLACK, Color::BLACK} },{' ', true, {Color::BLACK, Color::BLACK} },{' ', true, {Color::BLACK, Color::BLACK} },{'#', false, {Color::WHITE, Color::GREEN} },{'#', false, {Color::WHITE, Color::GREEN} },{'#', false, {Color::WHITE, Color::GREEN} },{'#', false, {Color::WHITE, Color::GREEN} },{'#', false, {Color::WHITE, Color::GREEN} },{'#', false, {Color::WHITE, Color::GREEN} },{'#', false, {Color::WHITE, Color::GREEN} },{'#', false, {Color::WHITE, Color::GREEN} }};
		m_plane[14] = {{'#', false, {Color::WHITE, Color::GREEN} },{' ', true, {Color::BLACK, Color::BLACK} },{' ', true, {Color::BLACK, Color::BLACK} },{' ', true, {Color::BLACK, Color::BLACK} },{' ', true, {Color::BLACK, Color::BLACK} },{' ', true, {Color::BLACK, Color::BLACK} },{' ', true, {Color::BLACK, Color::BLACK} },{' ', true, {Color::BLACK, Color::BLACK} },{' ', true, {Color::BLACK, Color::BLACK} },{' ', true, {Color::BLACK, Color::BLACK} },{' ', true, {Color::BLACK, Color::BLACK} },{' ', true, {Color::BLACK, Color::BLACK} },{' ', true, {Color::BLACK, Color::BLACK} },{' ', true, {Color::BLACK, Color::BLACK} },{' ', true, {Color::BLACK, Color::BLACK} },{'#', false, {Color::WHITE, Color::GREEN} }};
		m_plane[15] = {{'#', false, {Color::WHITE, Color::GREEN} },{'#', false, {Color::WHITE, Color::GREEN} },{'#', false, {Color::WHITE, Color::GREEN} },{'#', false, {Color::WHITE, Color::GREEN} },{'#', false, {Color::WHITE, Color::GREEN} },{'#', false, {Color::WHITE, Color::GREEN} },{'#', false, {Color::WHITE, Color::GREEN} },{'#', false, {Color::WHITE, Color::GREEN} },{'#', false, {Color::WHITE, Color::GREEN} },{'#', false, {Color::WHITE, Color::GREEN} },{'#', false, {Color::WHITE, Color::GREEN} },{'#', false, {Color::WHITE, Color::GREEN} },{'#', false, {Color::WHITE, Color::GREEN} },{'#', false, {Color::WHITE, Color::GREEN} },{'#', false, {Color::WHITE, Color::GREEN} },{'#', false, {Color::WHITE, Color::GREEN}}};

	}
#endif
// TODO: Generate map function Euan
private:
	MapPlane m_plane;
	std::pair<unsigned long, unsigned long> m_size; // width, height
};

#endif
