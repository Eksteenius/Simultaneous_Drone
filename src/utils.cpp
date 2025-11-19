#include "utils.h"

bool utils::pointOverlapBox(Vector2 point, Vector2 box_position, float box_width, float box_height)
{
	if ((point.x > box_position.x && point.x < box_position.x + box_width) &&
		(point.y > box_position.y && point.y < box_position.y + box_height))
	{
		return true;
	}
	return false;
}

bool utils::boxOverlapBox(std::pair<float, float> position_a, float width_a, float height_a,
	std::pair<float, float> position_b, float width_b, float height_b)
{
	if (position_a.first < (position_b.first + width_b)
		&& position_a.first + width_a > position_b.first
		&& position_a.second < position_b.second + height_b
		&& position_a.second + height_a > position_b.second)
	{
		return true;
	}
	return false;
}
