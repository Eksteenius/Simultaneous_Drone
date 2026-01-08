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
		if (_pathfinder->path_in_use == false)
		{
			//proximity_distance = size / 2.f;
			proximity_distance = 0.5f;

			if (_pathfinder->pathing_solved)
			{
				path_progress = _pathfinder->getPath().size() - 1;
			}
			_pathfinder->path_in_use = true;
		}

		Vector2 tile_coords = { -1, -1 };
		bool tile_valid = false;

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

void Drone::moveToPoint(Vector2 point, float dt)
{
	Vector2 direction = utils::directionToPoint(position, point);
	Vector2 unit_direction = utils::unitVector(direction);

	if(utils::magnitude(direction) > utils::magnitude(unit_direction))
	{
		position.x += unit_direction.x * speed * dt;
		position.y += unit_direction.y * speed * dt;
	}
	else
	{
		position.x += direction.x * speed * dt;
		position.y += direction.y * speed * dt;
	}
}
