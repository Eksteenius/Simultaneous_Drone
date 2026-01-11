
#ifndef RAVINES_PATHFINDER_H
#define RAVINES_PATHFINDER_H

#include <algorithm>

#include "utils.h"

#include "Cell.h"

class Pathfinder
{
public:
    Pathfinder() = default;
    Pathfinder(std::vector<Cell>& _cells);
	~Pathfinder();

	void AStar();
    float heuristic(Cell& _start_cell, Cell& _neighbor, Cell& _end_cell);

    void resetPathfinder();

    void setStartEndIndex(int start_index, int end_index);

    std::vector<std::reference_wrapper<Cell>>& getPath();
    std::vector<std::reference_wrapper<Cell>>& getClosedSet();

    std::vector<std::reference_wrapper<Cell>>& getLastSolvedPath();

    bool path_set = false;
    int search_iterations = 200;
    bool pathing_complete = false;
    bool pathing_solved = false;

    bool path_in_use = false;

private:
    int start_cell_index;
    int end_cell_index;

    std::vector<std::reference_wrapper<Cell>> path;
    std::vector<std::reference_wrapper<Cell>> last_solved_path;

    std::vector<std::reference_wrapper<Cell>> open_set;
    std::vector<std::reference_wrapper<Cell>> closed_set;

    std::vector<Cell>& cells;
};

#endif // RAVINES_PATHFINDER_H

