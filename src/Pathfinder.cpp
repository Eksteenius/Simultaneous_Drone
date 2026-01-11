
#include "Pathfinder.h"

Pathfinder::Pathfinder(std::vector<Cell>& _cells) : cells(_cells)
{
}

Pathfinder::~Pathfinder()
{
}

void Pathfinder::AStar()
{
    if (cells[start_cell_index].barrier == true || cells[end_cell_index].barrier == true) 
    {
        pathing_complete = true;
        pathing_solved = false;
    }
    else if (path_set == true)
    {
        if (!open_set.empty())
        {
            std::sort(open_set.begin(), open_set.end(), [](const Cell& a, const Cell& b) {
                if (a.f != b.f) {
                    return a.f > b.f;  // Sort by largest f values first
                }
                return a.g < b.g;  // Sort by smallest g value first when f is equal
                });

            Cell& current_cell = open_set.back();
            //current_cell.neighbors = open_set.back().get().neighbors;

            if (current_cell == cells[end_cell_index])
            {
                pathing_complete = true;
                pathing_solved = true;

                /// Find the path 
                path = {};
                Cell* temp_current = &current_cell;
                path.push_back(*temp_current);

                while (temp_current->previous != nullptr)
                {
                    path.push_back(*temp_current->previous);
                    temp_current = temp_current->previous;
                }
            }
            else
            {
                //open_set.remove(current_cell);
                open_set.pop_back();

                closed_set.push_back(current_cell);

                for (int i = 0; i < current_cell.neighbors.size(); i++)
                {
                    Cell& neighbor = current_cell.neighbors.at(i);

                    if (std::find(closed_set.begin(), closed_set.end(), neighbor) == closed_set.end() && neighbor.barrier == false)
                    {
                        bool diagonal_blocked = false;
                        if(neighbor.i != current_cell.i && neighbor.j != current_cell.j)
                        {
                            int difference_i = current_cell.i - neighbor.i;
                            int difference_j = current_cell.j - neighbor.j;

                            if(cells.at(current_cell.j * (int)sqrt(cells.size()) + current_cell.i - difference_i).barrier)
                            {
                                diagonal_blocked = true;
                            }

                            if (cells.at((current_cell.j - difference_j) * (int)sqrt(cells.size()) + current_cell.i).barrier)
                            {
                                diagonal_blocked = true;
                            }
                        }

                        if (!diagonal_blocked)
                        {
                            float tentative_g;

                            if (neighbor.i != current_cell.i && neighbor.j != current_cell.j)
                            {
                                tentative_g = current_cell.g + 1.4f;
                            }
                            else
                            {
                                tentative_g = current_cell.g + 1.0f;
                            }

                            bool new_path = false;

                            if (std::find(open_set.begin(), open_set.end(), neighbor) != open_set.end())
                            {
                                if (tentative_g < neighbor.g)
                                {
                                    neighbor.g = tentative_g;
                                    new_path = true;
                                }
                            }
                            else
                            {
                                neighbor.g = tentative_g;
                                new_path = true;

                                open_set.push_back(neighbor);
                            }

                            if (new_path == true)
                            {
                                neighbor.h = heuristic(current_cell, neighbor, cells[end_cell_index]);
                                neighbor.f = neighbor.g + neighbor.h;
                                neighbor.previous = &current_cell;
                            }
                        }
                    }
                }
            }
        }
        else
        {
            /// No solution
            pathing_complete = true;
            pathing_solved = false;
        }
    }

    if(pathing_solved)
    {
        last_solved_path = path;
    }
}

float Pathfinder::heuristic(Cell& _start_cell, Cell& _neighbor, Cell& _end_cell)
{
    Cell& neigh = _neighbor;
    Cell& end = _end_cell;
    Cell& start = _start_cell;

    /// The heuristic should try be a value as close to the actual shortest distance as possible.

    /// If a heuristic is too low it will search more than needed
    /// If a heuristic is too high it will become a greedy best first search

    ///Manhattan Heuristic With Dynamic Priority Intuitive
    int dx1 = std::abs(neigh.i - end.i);
    int dy1 = std::abs(neigh.j - end.j);
    int dx2 = std::abs(start.i - end.i);
    int dy2 = std::abs(start.j - end.j);
    float cross = (float)std::abs((dx1 * dy2) - (dx2 * dy1));
    float h = (float)(dx1 + dy1);
    //h += cross * 0.01;
    h *= 1 + (1 / (cells.size()));
    float d = h;
    return d;

    ///Manhattan Distance (Bad at diagonals)
    //int d = (std::abs(neigh.i - end.i) + std::abs(neigh.j - end.j));
    //return d;

    ///Standard Manhattan Heuristic
    //int dx = std::abs(neigh.i - end.i);
    //int dy = std::abs(neigh.j - end.j);
    //int d = 1;
    //return d * (dx + dy);

    ///Euclidean Heuristic (too low) (shortest path will almost never be this short a distance)
    //float d = std::sqrt(std::pow(neigh.i - neigh.j, 2) + std::pow(end.i - end.j, 2));
    //return d;


    ///Other
    //return 1;
}

void Pathfinder::resetPathfinder()
{
    for (Cell& cell : cells)
    {
            cell.f = 0;
            cell.g = 0;
            cell.h = 0;
            cell.previous = nullptr;
    }

    open_set.clear();
    closed_set.clear();
    path.clear();
    
    pathing_complete = false;
    pathing_solved = false;

    open_set.push_back(cells[start_cell_index]);

    path_in_use = false;
}

void Pathfinder::setStartEndIndex(int start_index, int end_index)
{
    start_cell_index = start_index;
    end_cell_index = end_index;

    resetPathfinder();

    path_set = true;
}

std::vector<std::reference_wrapper<Cell>>& Pathfinder::getPath()
{
    return path;
}

std::vector<std::reference_wrapper<Cell>>& Pathfinder::getClosedSet()
{
    return closed_set;
}

std::vector<std::reference_wrapper<Cell>>& Pathfinder::getLastSolvedPath()
{
    return last_solved_path;
}
