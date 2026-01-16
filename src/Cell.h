#pragma once

#include <iostream>
#include <vector>

class Cell
{
public:
	Cell(int _i, int _j);
	~Cell();

	void resetPathData();

	void addNeighbors(std::vector<Cell>&, int rows, int columns);

	std::vector<std::reference_wrapper<Cell>> neighbors;

	int i = 0;
	int j = 0;

	bool barrier = false;

	float f = 0;
	float g = 0;
	float h = 0;

	std::shared_ptr<Cell> previous = nullptr;

	bool operator==(const Cell& other) const 
	{
		return i == other.i && j == other.j;
	}

private:

};
