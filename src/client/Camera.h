#ifndef ROGUELIKE_CAMERA_H_INCLUDED
#define ROGUELIKE_CAMERA_H_INCLUDED
#include <string>
#include <utility>
#include <vector>
#include <time.h>
#include <memory>
#include <mutex>
#include <math.h>
#include <algorithm>

#include "../server/Map.h"
#include "Renderer.h"
#include <chrono>

// depth of 2.5D rendering
constexpr float RENDER_DEPTH = 16.0f;

class Camera final : public Object, public Drawable
{
public:
	Camera(char blank_char, Map* map, std::pair<long, long> start_position, std::pair<long, long> minimap_size);

	// Draws the 3D view and minimap to the given renderer
    void draw(Renderer* renderer);
    
    // Draws the minimap to the given renderer
	void draw_minimap(Renderer* renderer);

    // Draws the 3D view to the given renderer
	void draw_3D(Renderer* renderer);

	void rotate(float angle){
		m_angle+=angle;
	}

	void move(Direction dir);

private:
    // Minimap size (width x height)
	std::pair<float, float> m_minimap_size;
// 2.5D rendering
	float m_angle = 0.f; // angle for ray_casting
	float m_fov =  3.14159f / 4.0f; // field of view
// 2.5D rendering
	char m_blank_char; // character to put if there is nothing to render on position
	Map* m_map; // map which camera is observing
	std::mutex pos_mutex; // prevent changing position during rendering a frame
	std::vector<std::shared_ptr<Object>> objects_in_range;

	// get objects in range of camera view and keep it in objects_in_range vecror
	void get_objects_in_range(std::pair<long, long> range_y, std::pair<long, long> range_x);
};

#endif
