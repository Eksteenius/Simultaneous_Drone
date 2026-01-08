#pragma once

#include <raylib.h>

#include "utils.h"

#include "Pathfinder.h"


class Drone
{
public:
	Drone();
	~Drone();

	void moveOnPath(std::shared_ptr<Pathfinder> _pathfinder, float size, float dt);
	void moveToPoint(Vector2 point, float dt);

	/// Pathfinding
	float proximity_distance = 0;
	int path_progress = -1;

	float speed = 100;
	Vector2 position = {0, 0};

private:

};

