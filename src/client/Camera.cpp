#include "Camera.h"
#include <iostream>

Camera::Camera()
{}

Camera::Camera(char blank_char, Map* map, std::pair<long, long> start_position):
	m_blank_char(blank_char), m_map(map), Object('\0', Direction::NORTH, false, start_position)
{}

void Camera::move(const Direction dir){
	std::lock_guard<std::mutex> lock (pos_mutex);

	switch (dir)
	{
	case Direction::NORTH:
		m_position.first += sinf(m_angle);
		m_position.second += cosf(m_angle);
		if(m_position.first)
		break;
	case Direction::SOUTH:
		m_position.first -= sinf(m_angle);
		m_position.second -= cosf(m_angle);
	default:
		break;
	}
}

void Camera::get_objects_in_range(std::pair<long, long> range_y, std::pair<long, long> range_x){
	for (Object* obj : m_map->objects){
		if (obj->get_visibility() && 
			obj->get_position().second >= range_y.first && 
			obj->get_position().second < range_y.second &&
			obj->get_position().first >= range_x.first &&
			obj->get_position().first < range_x.second) {
				objects_in_range.push_back(obj);
			}
	}
}

void Camera::draw(Renderer* renderer) {
    draw_3D(renderer);
    draw_minimap(renderer);
}

void Camera::draw_minimap(Renderer* renderer)	{
    unsigned int viewportWidth = renderer->getWidth();
    unsigned int viewportHeight = renderer->getHeight();
    
	// plane of the map
	MapPlane* plane = m_map->get_map_plane();
	// size of the map
	std::pair<unsigned long, unsigned long> map_size = m_map->get_map_size();

	// lock camera position to avoid changing position during rendering
	std::lock_guard<std::mutex> lock (pos_mutex);

    int y_offset = viewportHeight - MINIMAP_HEIGHT;
    int start_i = -1 * (MINIMAP_HEIGHT / 2) + m_position.second;
    int start_j = -1 * (MINIMAP_WIDTH / 2) + m_position.first;
    int end_i = MINIMAP_HEIGHT / 2 + m_position.second;
    int end_j = MINIMAP_WIDTH / 2 + m_position.first;
    const Formating default_formatting {
        Color::NO_COLOR,
        Color::NO_COLOR
    };

	// get objects in range of camera
	get_objects_in_range({start_i, end_i}, {start_j, end_j});
    
	for (int i = start_i; i < end_i; i++)
	{
		for (int j = start_j; j < end_j; j++)
		{
			if (i >= 0 && j >= 0 && i < map_size.second && j < map_size.first)
			{
				// flag to check if on current cell is an object
				bool object = false;
				// check all objects in range if there is a one on current position
				for (Object* obj : objects_in_range){
                    if (obj->get_visibility() && obj->get_position().second == i && obj->get_position().first == j){
                        // Draw object to cell
						//auto temp = obj->get_position();
						//obj->get_position().first = obj->get_position().second;
						//obj->get_position().second = temp.first;
                        renderer->draw_cell(j - start_j, i - start_i + y_offset, obj->get_char(), obj->get_formating());
                        object = true;
                    }
				}
				
				if(!object) {
                    // Draw tile to cell
                    renderer->draw_cell(j - start_j, i - start_i + y_offset, (*plane)[j][i].character, (*plane)[j][i].formating);
                }
			}
			else
			{
                // Draw blank cell
                renderer->draw_cell(j - start_j, i - start_i + y_offset, ' ', default_formatting);
			}
		}
	}
	objects_in_range.clear();
}

// some parts of code adapted from https://github.com/OneLoneCoder/CommandLineFPS/blob/master/CommandLineFPS.cpp
void Camera::draw_3D(Renderer* renderer) {
    unsigned int viewportWidth = renderer->getWidth();
    unsigned int viewportHeight = renderer->getHeight();
    
	int start_i = -1 * (MINIMAP_HEIGHT / 2) + m_position.second;
    int start_j = -1 * (MINIMAP_WIDTH / 2) + m_position.first;
    int end_i = MINIMAP_HEIGHT / 2 + m_position.second;
    int end_j = MINIMAP_WIDTH / 2 + m_position.first;

	// get objects in range of camera
	get_objects_in_range({start_i, end_i}, {start_j, end_j});

	//lock the position values to avoid changing it while rendering a frame
	std::lock_guard<std::mutex> lock (pos_mutex);
	// plane of the map
	MapPlane* plane = m_map->get_map_plane();
	// size of the map
	std::pair<unsigned long, unsigned long> map_size = m_map->get_map_size();

	std::vector<std::string> vec_map_to_render(viewportWidth, std::string(viewportHeight, ' '));
    
    const Formating default_formatting {
        Color::NO_COLOR,
        Color::NO_COLOR
    };

	// cast a ray for every column in a screen
	for(size_t i = 0; i<viewportWidth; i++){
		// For each column, calculate the projected ray angle into world space
		float fRayAngle = (m_angle - m_fov/2.0f) + ((float)i / (float)viewportWidth) * m_fov;
		
		float step = 0.1f; // step for incrementing distance of ray				
		float dist = 0.0f; // distance of ray

		bool wall_flag = false;		// true if there is a wall on the path of ray
		bool item_flag = false;		// true if there is an object on the path of ray
		bool boundary_flag = false;		// true if ray hit between the walls

		float fEyeX = sinf(fRayAngle); // Unit vector for ray in player space
		float fEyeY = cosf(fRayAngle);

		while(!wall_flag && dist<RENDER_DEPTH){
			// increment the ray distance
			dist += step;

			// current position of ray
			int ray_pos_x = static_cast<int>(static_cast<float>(m_position.first) + fEyeX * dist);
			int ray_pos_y = static_cast<int>(static_cast<float>(m_position.second) + fEyeY * dist);

			// check if ray is still in the map
			if (ray_pos_x < 0 || ray_pos_x >= map_size.first || ray_pos_y < 0 || ray_pos_y >= map_size.second) {
				break;
			}
			// check if ray hitted the wall
			else if(!(*plane)[ray_pos_x][ray_pos_y].accesible) {
				wall_flag = true;
				char render_char;
				std::string color_code;
				// set character equivalent to distance from wall
				if (dist <= RENDER_DEPTH / 4.0f){
					render_char = '#';	
				}
				else if (dist < RENDER_DEPTH / 3.0f){
					render_char = '8';
					}
				else if (dist < RENDER_DEPTH / 2.0f){
					render_char = '-';
				}
				else if (dist < RENDER_DEPTH){
					render_char = '.';
				}
					
				std::vector<std::pair<float, float>> p;

				// Test each corner of hit tile, storing the distance from
				// the player, and the calculated dot product of the two rays
				for (int tx = 0; tx < 2; tx++)
					for (int ty = 0; ty < 2; ty++)
					{
						// Angle of corner to eye
						float vy = static_cast<float>(ray_pos_y) + ty - m_position.second;
						float vx = static_cast<float>(ray_pos_x) + tx - m_position.first;
						float d = sqrt(vx*vx + vy*vy); 
						float dot = (fEyeX * vx / d) + (fEyeY * vy / d);
						p.push_back(std::make_pair(d, dot));
					}
				// Sort Pairs from closest to farthest
				std::sort(p.begin(), p.end(), [](const std::pair<float, float> &left, const std::pair<float, float> &right) {return left.first < right.first; });
					
				float fBound = 0.01;
				if (acos(p.at(0).second) < fBound) boundary_flag = true;
				if (acos(p.at(1).second) < fBound) boundary_flag = true;
				if (acos(p.at(2).second) < fBound) boundary_flag = true;

				unsigned short len_of_column = static_cast<int>(viewportHeight-dist*2.0f);
				if(len_of_column>=viewportHeight) len_of_column = viewportHeight-1;

					if(!boundary_flag) {
						vec_map_to_render[i].replace((viewportHeight-len_of_column)/2,
						len_of_column,
						std::string(len_of_column,
						render_char));
						(*plane)[ray_pos_x][ray_pos_y].character = 'x';
					}
				}
			}
	}

	// draw frame to renderer
	for(int i = 0; i<viewportHeight; i++){
		for(int j = 0; j<viewportWidth; j++){
            renderer->draw_cell(j, i, vec_map_to_render[j][i], default_formatting);
		}
	}
	objects_in_range.clear();
}
// end of adapted code
