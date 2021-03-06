#include "Camera.h"
#include <iostream>

Camera::Camera(char blank_char, Map* map, std::pair<long, long> start_position, std::pair<long, long> minimap_size):
	Object('\0', Direction::NORTH, false, start_position), m_minimap_size(minimap_size), m_blank_char(blank_char), m_map(map)
{}

void Camera::move(Direction dir){
	std::lock_guard<std::mutex> lock (pos_mutex);

	int degrees = static_cast<int>(m_angle * (180.0/3.141592653589793238463));
	degrees = degrees%360;
	if(degrees<0)
		degrees=-1*degrees;
	//std::cout<<degrees<<std::endl;
	switch (dir)
	{
	case Direction::NORTH:
		if(degrees>=315 || degrees <= 45)
			m_position.second++;
		else if (degrees>=45 && degrees<=135)
			m_position.first++;
		else if (degrees>=135 && degrees<=225)
			m_position.second--;
		else 
			m_position.first--;
		break;
	case Direction::SOUTH:
		if(degrees>=315 || degrees <= 45)
			m_position.second--;
		else if (degrees>=45 && degrees<=135)
			m_position.first--;
		else if (degrees>=135 && degrees<=225)
			m_position.second++;
		else 
			m_position.first++;
		break;
	default:
		break;
	}
}

eDirection Camera::getMapDirection(Direction dir) {
    // Copy-paste from Camera::move above, slightly modified
    std::lock_guard<std::mutex> lock (pos_mutex);

    int degrees = static_cast<int>(m_angle * (180.0/3.141592653589793238463));
    degrees = degrees%360;
    if(degrees<0)
        degrees=-1*degrees;
    //std::cout<<degrees<<std::endl;
    switch (dir)
    {
    case Direction::NORTH:
        if(degrees>=315 || degrees <= 45){
            return eDirection::DOWN;
			}
        else if (degrees>=45 && degrees<=135){
            return eDirection::LEFT;
		}
        else if (degrees>=135 && degrees<=225){
            return eDirection::UP;
		}
        else {
            return eDirection::RIGHT;
		}
        break;
    case Direction::SOUTH:
        if(degrees>=315 || degrees <= 45)
            return eDirection::UP;
        else if (degrees>=45 && degrees<=135)
            return eDirection::RIGHT;
        else if (degrees>=135 && degrees<=225)
            return eDirection::DOWN;
        else 
            return eDirection::LEFT;
        break;
    default:
        break;
    }
    return eDirection::STOP;
}

void Camera::get_objects_in_range(std::pair<long, long> range_y, std::pair<long, long> range_x){
	for (auto obj : m_map->objects){
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

    int y_offset = viewportHeight-m_minimap_size.second;
    int start_i = -1 * (m_minimap_size.second / 2) + m_position.second;
    int start_j = -1 * (m_minimap_size.first / 2) + m_position.first;
    int end_i = m_minimap_size.second / 2 + m_position.second;
    int end_j = m_minimap_size.first / 2 + m_position.first;
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
				for (auto obj : objects_in_range){
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

	// get objects in range of camera
	get_objects_in_range({-1 * (m_minimap_size.second / 2) + m_position.second, m_minimap_size.second / 2 + m_position.second},
	{-1 * (m_minimap_size.first / 2) + m_position.first, m_minimap_size.first / 2 + m_position.first});

	using obj_to_render = std::pair<std::shared_ptr<Object>, std::pair<float, size_t>>;  // <object, <distance, viewport i>>
	std::vector<obj_to_render> objects_to_render;

	//std::cout<<objects_in_range.size()<<' ';

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
					}
				}
				for(auto it = objects_in_range.begin(); it != objects_in_range.end();){
					auto objX = (*it)->get_position().first;
					auto objY = (*it)->get_position().second;
					if (objX == ray_pos_x && objY == ray_pos_y
						&& (*it)->get_type() != ObjectType::PLAYER && objX >= 0 && objX < plane->size() && objY >= 0 && objY < (*plane)[0].size()) {
						objects_to_render.push_back({ (*it), {dist, i} });
						it = objects_in_range.erase(it);
					}
					else
						it++;
				}
			}
	}

	auto Pred = [](obj_to_render a, obj_to_render b) {
		return (a.first==b.first) ? true : false;
	};
	// draw frame to renderer
	for(int i = 0; i<viewportHeight; i++){
		for(int j = 0; j<viewportWidth; j++){
			if(vec_map_to_render[j][i]=='#')
			    renderer->draw_cell(j, i, vec_map_to_render[j][i], {Color::WHITE, Color::WHITE});
			else if(vec_map_to_render[j][i]=='8')
				renderer->draw_cell(j, i, '#', {Color::BLACK, Color::WHITE});
			else if(vec_map_to_render[j][i]=='-')
				renderer->draw_cell(j, i, '#', {Color::WHITE, Color::NO_COLOR});
			else
            	renderer->draw_cell(j, i, vec_map_to_render[j][i], default_formatting);
		}
	}
	// filter vector to have unique objects
	std::vector<obj_to_render>::iterator it; 
    it = std::unique(objects_to_render.begin(), objects_to_render.end(), Pred); 
    objects_to_render.resize(std::distance(objects_to_render.begin(), it)); 
	for(obj_to_render obj : objects_to_render){
		float scaling;
		if (obj.second.first <= RENDER_DEPTH / 4.0f){
			scaling = 1.0f;	
		}
		else if (obj.second.first < RENDER_DEPTH / 3.0f){
			scaling = 2.f;
		}
		else if (obj.second.first < RENDER_DEPTH / 2.0f){
			scaling = 3.f;
		}
		else if (obj.second.first < RENDER_DEPTH){
			scaling = 4.f;
		}
		else
            continue; // Skip this object, too far away (or else, scaling would be uninitialised, xoxo - raf)
		float w = obj.first->get_texture().get_plane().size()/scaling;
		float tw = obj.first->get_texture().get_plane().size();
		unsigned short len_of_column = static_cast<int>((viewportHeight-obj.second.first)/1.75f);
		if(len_of_column>=viewportHeight) len_of_column = viewportHeight-1;
		for(int i = obj.second.second, y = 0; i<obj.second.second + obj.first->get_texture().get_plane().size()/scaling && i<viewportWidth; ++i, ++y){
			for(int j = len_of_column, x = 0; j<len_of_column + obj.first->get_texture().get_plane().size()/scaling && j<viewportWidth; ++j, ++x){
				try{
					renderer->draw_cell(i, j, obj.first->get_texture().get_plane()[static_cast<int>(x/w*tw)][static_cast<int>(y/w*tw)].character,
					obj.first->get_texture().get_plane()[static_cast<int>(x/w*tw)][static_cast<int>(y/w*tw)].formating);
				}
				catch(...){
					throw;
				}
			}
		}
	}
}
// end of adapted code
