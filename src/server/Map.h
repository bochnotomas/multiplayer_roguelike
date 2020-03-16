#ifndef ROGUELIKE_MAP_H_INCLUDED
#define ROGUELIKE_MAP_H_INCLUDED
#include <vector>
#include <utility>
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
	Map(MapPlane& plane_) : m_plane(plane_), m_size({ plane_.size(), plane_.size() }) {}
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
