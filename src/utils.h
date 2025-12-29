#pragma once
#include <raylib.h>

#include <iostream>

namespace utils
{
	bool pointOverlapBox(Vector2 point, Vector2 box_position, float box_width, float box_height);
	bool boxOverlapBox(std::pair<float, float> position_a, float width_a, float height_a,
		std::pair<float, float> position_b, float width_b, float height_b);

	float randomFromRange(float min, float max);

	/// Uses grid root size
	int coordsToIndex(int x, int y, int size);
	int coordsToIndex(Vector2 coords, int size);

	Vector2 globalToCoords(Vector2 coords, float size);
}