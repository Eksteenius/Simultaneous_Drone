#pragma once

#include <raylib.h>
#include <raymath.h>

#include "utils.h"

#include "Pathfinder.h"


class Drone
{
public:
	Drone();
	~Drone();

	Vector2 center();

	/// Movement
	void moveToPoint(Vector2 point, float dt);
	
	/// Pathfinding
	void moveOnPath(std::shared_ptr<Pathfinder> _pathfinder, float size, float dt);

	float size = 1;
	float rotation = 0;
	Vector2 position = { 0, 0 };
	float speed = 500;
	float rotation_speed = 180;

	float proximity_distance = 0.0f;

	Vector2* getTilePathing();



	

private:

	int path_progress = -1;
	Vector2 tile_coords = { -1, -1 };
	bool tile_valid = false;
};

