#pragma once

#include <raylib.h>

class Raycast
{
public:
	Raycast(Vector2 _start, Vector2 _end, float _length);
	~Raycast();

	Vector2 start;
	Vector2 end;
	float length;
private:
};
