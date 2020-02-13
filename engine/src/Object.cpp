#include "Object.h"


char Object::get_char() const
{
	return m_character;
}

Color Object::get_color() const
{
	return m_color;
}

bool Object::get_visibility() const
{
	return m_visibility;
}

std::pair<long, long> Object::get_position() const
{
	return m_position;
}

void Object::move(const Direction dir)
{
	switch (dir)
	{
	case Direction::SOUTH:
		m_position.second++;
		break;
	case Direction::EAST:
		m_position.first++;
		break;
	case Direction::NORTH:
		m_position.second--;
		break;
	case Direction::WEST:
		m_position.first--;
		break;
	default:
		throw;
		break;
	}
}

void Object::set_position(const std::pair<long, long> new_position)
{
	m_position = new_position;
}
