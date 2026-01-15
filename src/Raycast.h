#pragma once

#include <raylib.h>

struct Raycast
{
	Vector2 start;
	Vector2 end;

	bool collided = false;
	int collider_index;
	Vector2 collision = { 0,0 };
	float distance = 0;
};
