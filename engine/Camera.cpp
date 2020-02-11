#include "Camera.h"

Camera::Camera()
{
}

Camera::Camera(char blank_char, Map& map, std::pair<long, long> start_position):
	m_blank_char(blank_char), m_map(map), Object('\0', Direction::NORTH, false, start_position)
{
}

std::string Camera::get_to_render()
{
	std::vector<std::string> map_to_render(RENDER_HEIGHT, std::string(RENDER_WIDTH, m_blank_char));

	MapPlane* plane = m_map.get_map_plane();
	std::pair<unsigned long, unsigned long> map_size = m_map.get_map_size();

	size_t x = 0, y = 0;
	for (int i = -1 * (RENDER_HEIGHT / 2) + m_position.second; i < RENDER_HEIGHT / 2 + m_position.second; i++)
	{
		for (int j = -1 * (RENDER_WIDTH / 2) + m_position.first; j < RENDER_WIDTH / 2 + m_position.first; j++)
		{
			if (i >= 0 && j >= 0 && i < map_size.second && j < map_size.first)
			{
				map_to_render[y][x] = (*plane)[i][j].character;
			}
			for (Object* obj : m_map.objects)
			{
				if (obj->get_visibility() && obj->get_position().second == i && obj->get_position().first == j)
					map_to_render[y][x] = obj->get_char();
			}
			x++;
		}
		y++;
		x = 0;
	}

	plane = nullptr;
	delete plane;

	std::string str_map_to_render;
	for (std::string row : map_to_render)
	{
		str_map_to_render.append(std::string(row.begin(), row.end()));
		str_map_to_render += '\n';
	}

	return {str_map_to_render};
}
