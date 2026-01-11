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
	void moveToPoint(Vector2 point, float amount, float dt);
	/// Pathfinding
	void moveOnPath(std::shared_ptr<Pathfinder> _pathfinder, float size, float dt);

	float size = 1;
	float rotation = 0;
	Vector2 position = { 0, 0 };
	float speed = 500;
	float rotation_speed = 180;

	float proximity_distance = 5.0f;

	Vector2 getCurrentPathing();
	bool path_valid = false;

private:
	void setPathing(std::shared_ptr<Pathfinder> _pathfinder, int index);

	int path_progress = -1;
	Vector2 path_coords = { 0, 0 };
};

