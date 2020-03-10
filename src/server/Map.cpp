#include  "Map.h"
#include <iostream>

Map::Map()
{
	m_plane = MapPlane(0);
	m_size = {0, 0};
}

Map::~Map()
{
	for (Object* obj : objects)
	{
		delete obj;
		obj = nullptr;
	}
}

void Map::update_objects()
{
	for (Object* obj : objects)
	{
		obj->update();
	}
}

MapPlane* Map::get_map_plane()
{
	return &m_plane;
}

std::pair<unsigned long, unsigned long> Map::get_map_size()
{
	return m_size;
}
