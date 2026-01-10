#include "Drone.h"

Drone::Drone()
{
}

Drone::~Drone()
{
}

Vector2 Drone::center()
{
	return { position.x + size, position.y + size };
}

void Drone::moveOnPath(std::shared_ptr<Pathfinder> _pathfinder, float size, float dt)
{
	if (_pathfinder->pathing_complete)
	{
		/// Pathfinder will set path in use to false when path is reset
		if (_pathfinder->path_in_use == false)
		{
			if (_pathfinder->pathing_solved)
			{
				path_progress = _pathfinder->getPath().size() - 1;
			}
			_pathfinder->path_in_use = true;
		}

		tile_coords = { -1, -1 };
		tile_valid = false;

		if (_pathfinder->pathing_solved && _pathfinder->getPath()[path_progress].get().barrier == false)
		{
			tile_coords = Vector2(_pathfinder->getPath()[path_progress].get().i, _pathfinder->getPath()[path_progress].get().j);
			tile_valid = true;
		}
		else if (_pathfinder->getLastSolvedPath().size() > 0 && _pathfinder->getLastSolvedPath()[path_progress].get().barrier == false)
		{
			tile_coords = Vector2(_pathfinder->getLastSolvedPath()[path_progress].get().i, _pathfinder->getLastSolvedPath()[path_progress].get().j);
			tile_valid = true;
		}

		if (tile_valid)
		{
			Vector2 coords = Vector2(tile_coords.x * size, tile_coords.y * size);

			Vector2 distance = { coords.x - position.x, coords.y - position.y };

			if (std::abs(utils::magnitude(distance)) > proximity_distance)
			{
				moveToPoint(coords, dt);
			}
			else
			{
				if (path_progress > 0)
				{
					path_progress--;
				}
			}
		}
	}
}

Vector2* Drone::getTilePathing()
{
	if (tile_valid)
	{
		return &tile_coords;
	}
	return nullptr;
}

void Drone::moveToPoint(Vector2 point, float dt)
{
	Vector2 direction = utils::directionToPoint(position, point);
	Vector2 unit_direction = utils::unitVector(direction);

	/// Rotation
	float direction_angle = fmodf(atan2(direction.y, direction.x) * 180.0f / PI, 360.0f);
	if (direction_angle < 0) direction_angle += 360.0f;

	if(rotation != direction_angle)
	{
		float angle_delta = direction_angle - rotation;

		//// Wrap difference to [-180, 180]
		//if (angle_delta > 180.0f) angle_delta -= 360.0f;
		//if (angle_delta < -180.0f) angle_delta += 360.0f;

		if (abs(angle_delta) <= rotation_speed * dt)
		{
			rotation = direction_angle;
		}
		else
		{
			if (angle_delta > 180)
			{
				rotation += rotation_speed * dt;
			}
			else
			{
				rotation -= rotation_speed * dt;
			}

			rotation = fmodf(rotation, 360.0f);
			if (rotation < 0) rotation += 360.0f;
		}
	}
	else
	{
		/// Movement
		Vector2 movement = Vector2Scale(unit_direction, speed * dt);
		if (utils::magnitude(movement) < utils::magnitude(direction))
		{
			position = Vector2Add(position, movement);
		}
		else
		{
			position = point;
		}
	}
}
