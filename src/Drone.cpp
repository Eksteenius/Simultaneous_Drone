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
			else
			{
				path_progress = _pathfinder->getLastSolvedPath().size() - 1;
			}
			_pathfinder->path_in_use = true;

			setPathing(_pathfinder, path_progress);
		}

		if (path_valid)
		{
			Vector2 coords = Vector2(path_coords.x * size, path_coords.y * size);
			Vector2 distance = { coords.x - position.x, coords.y - position.y };

			if (std::abs(utils::magnitude(distance)) > proximity_distance)
			{
				moveToPoint(coords, speed * dt, dt);
			}
			else if (path_progress > 0) /// Within proximity
			{
				path_progress--;

				/// Set next tile 
				setPathing(_pathfinder, path_progress);

				/// move using remaining speed to prevent abrupt stops when reaching proximity distance
				float unused_movement = (speed * dt) - (proximity_distance - utils::magnitude(distance));
				if (unused_movement > 0)
				{
					coords = Vector2(path_coords.x * size, path_coords.y * size);
					moveToPoint(coords, unused_movement, dt);
				}
			}
			else
			{
				path_valid = false; /// there are no tiles left for pathfinding
			}
		}
	}
}

Vector2 Drone::getCurrentPathing()
{
	return path_coords;
}

void Drone::setPathing(std::shared_ptr<Pathfinder> _pathfinder, int index)
{
	path_valid = false;
	if (_pathfinder->pathing_solved && _pathfinder->getPath()[path_progress].get().barrier == false)
	{
		path_coords = Vector2(_pathfinder->getPath()[path_progress].get().i, _pathfinder->getPath()[path_progress].get().j);
		path_valid = true;
	}
	else if (_pathfinder->getLastSolvedPath().size() > 0 && _pathfinder->getLastSolvedPath()[path_progress].get().barrier == false)
	{
		path_coords = Vector2(_pathfinder->getLastSolvedPath()[path_progress].get().i, _pathfinder->getLastSolvedPath()[path_progress].get().j);
		path_valid = true;
	}
}

void Drone::moveToPoint(Vector2 point, float amount, float dt)
{
	Vector2 direction = utils::directionToPoint(position, point);
	Vector2 unit_direction = utils::unitVector(direction);

	float direction_angle = utils::AngleFromVector(direction);
	if (direction_angle < 0) direction_angle += 360;

	
	if(rotation != direction_angle)
	{
		/// Rotation
		float rotation_delta = direction_angle - rotation;
		/// Wrap difference within [-180, 180]
		if (rotation_delta > 180.0f) rotation_delta -= 360.0f;
		else if (rotation_delta < -180.0f) rotation_delta += 360.0f;

		if (abs(rotation_delta) <= rotation_speed * dt)
		{
			rotation = direction_angle;
		}
		else if (rotation_delta >= 0)
		{
			rotation = fmodf(rotation + rotation_speed * dt, 360.f);
		}
		else
		{
			rotation = fmodf(rotation - rotation_speed * dt, 360.f);
		}

		if (rotation < 0) rotation += 360;
	}
	
	if(rotation == direction_angle)
	{
		/// Movement
		Vector2 movement = Vector2Scale(unit_direction, amount);
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