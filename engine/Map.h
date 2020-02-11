#pragma once
#include <vector>
#include <utility>
#include "Object.h"

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
		m_plane = MapPlane(height, std::vector<MapPoint>(width, { ' ', true, Color::WHITE }));
		m_plane[0] = std::vector<MapPoint>(width, { '#', false, Color::LIGHT_PURPLE });
		m_plane[height-1] = std::vector<MapPoint>(width, { '#', false, Color::LIGHT_PURPLE });

		for (size_t i = 1; i < height; i++) {
			m_plane[i][0] = { '#', false, Color::LIGHT_PURPLE };
			m_plane[i][width-1] = { '#', false, Color::LIGHT_PURPLE };
		}
	}
#endif
// TODO: Generate map function Euan
private:
	MapPlane m_plane;
	std::pair<unsigned long, unsigned long> m_size; // width, height
};