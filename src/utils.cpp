#include "utils.h"

#include <random>
#include <cmath>

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

float utils::randomFromRange(float min, float max)
{
	std::random_device rd; // obtain a random number from hardware
	std::mt19937 gen(rd()); // seed the generator
	std::uniform_real_distribution<> distr(min, max); // define the range

	return distr(gen);
}

int utils::coordsToIndex(int x, int y, int size)
{
	return x + y * size;
}

int utils::coordsToIndex(Vector2 coords, int size)
{
	return coords.x + coords.y * size;
}

Vector2 utils::globalToCoords(Vector2 coords, float size)
{
	return Vector2((int)(coords.x / size), (int)(coords.y / size));
}

bool utils::isKeyVectorDown(std::vector<KeyboardKey> keys)
{
	for (KeyboardKey& key : keys)
	{
		if (IsKeyDown(key))
		{
			return true;
		}
	}
	return false;
}
