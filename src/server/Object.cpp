#include "Object.h"


char Object::get_char()
{
	return m_character;
}

Formating Object::get_formating() const
{
	return m_formating;
}

bool Object::get_visibility() const
{
	return m_visibility;
}

Texture Object::get_texture() const
{
	return m_texture;
}

std::pair<long, long> Object::get_position() const
{
	return {static_cast<long>(m_position.first), static_cast<long>(m_position.second)};
}

Direction Object::get_direction() const
{
    return m_dir;
}

ObjectType Object::get_type() const
{
    return m_type;
}


void Object::move(const Direction dir)
{
	switch (dir)
	{
	case Direction::SOUTH:
		m_position.second+=1;
		break;
	case Direction::EAST:
		m_position.first+=1;
		break;
	case Direction::NORTH:
		m_position.second-=1;
		break;
	case Direction::WEST:
		m_position.first-=1;
		break;
	default:
		throw;
		break;
	}
}

void Object::set_position(const std::pair<int, int> new_position)
{
	m_position = new_position;
}
