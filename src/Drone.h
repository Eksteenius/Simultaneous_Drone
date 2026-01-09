#pragma once

#include <raylib.h>

#include "utils.h"

#include "Pathfinder.h"


class Drone
{
public:
	Drone();
	~Drone();

	Vector2 center();

	float speed = 100;
	Vector2 position = { 0, 0 };
	float size = 1;

	/// Pathfinding
	void moveOnPath(std::shared_ptr<Pathfinder> _pathfinder, float size, float dt);

	Vector2* getTilePathing();

	/// Movement
	void moveToPoint(Vector2 point, float dt);

	float proximity_distance = 0;

private:

	int path_progress = -1;
	Vector2 tile_coords = { -1, -1 };
	bool tile_valid = false;
};

