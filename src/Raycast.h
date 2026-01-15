#pragma once

#include <raylib.h>

class Raycast
{
public:
	Raycast(Vector2 _start, Vector2 _end);
	Raycast();
	~Raycast();

	Vector2 start;
	Vector2 end;

	bool collided = false;
	Vector2 collision = { 0,0 };
	float distance = 0;
private:
};
