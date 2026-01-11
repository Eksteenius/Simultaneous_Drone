#pragma once
#include <raylib.h>

#include <iostream>
#include <vector>

namespace utils
{
	/// Collisions
	bool pointOverlapBox(Vector2 point, Vector2 box_position, float box_width, float box_height);
	bool boxOverlapBox(std::pair<float, float> position_a, float width_a, float height_a,
					   std::pair<float, float> position_b, float width_b, float height_b);

	/// Random number generation
	float randomFromRange(float min, float max);

	/// Uses grid root size
	bool coordsWithinGrid(Vector2 coords, float grid_size);
	int coordsToIndex(int x, int y, int grid_size);
	int coordsToIndex(Vector2 coords, int grid_size);
	/// Uses grid rect size
	Vector2 globalToCoords(Vector2 global, float rect_size);
	Vector2 coordsToGlobal(Vector2 coords, float rect_size);

	Vector2 center(Vector2 position, float size);

	/// Math
	float magnitude(Vector2 vector);
	Vector2 unitVector(Vector2 vector);
	Vector2 directionToPoint(Vector2 position, Vector2 point);

	/// Check if any of multiple keys are down
	bool isKeyVectorDown(std::vector<KeyboardKey> keys);
	bool isKeyVectorReleased(std::vector<KeyboardKey> keys);

	bool isAnyMouseButtonReleased();
}