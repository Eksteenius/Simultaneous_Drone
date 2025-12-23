
#include "Cell.h"

Cell::Cell(int _i, int _j)
{
    i = _i;
    j = _j;
}

Cell::~Cell()
{
}

void Cell::addNeighbors(std::vector<Cell>& cells, int rows, int columns)
{
    /// adds surrounding cells and makes sure not to add cells that do not exist on the edges
    neighbors.clear();

    if (j > 0)
    {
        neighbors.push_back(cells[i + (j - 1) * rows]);

        /// 0X0
        /// 0#0
        /// 000
    }
    if (i > 0)
    {
        neighbors.push_back(cells[(i - 1) + j * rows]);

        /// 000
        /// X#0
        /// 000
    }
    if (i < rows - 1)
    {
        neighbors.push_back(cells[(i + 1) + j * rows]);

        /// 000
        /// 0#X
        /// 000
    }
    if (j < columns - 1)
    {
        neighbors.push_back(cells[i + (j + 1) * rows]);

        /// 000
        /// 0#0
        /// 0X0
    }

    /// Diagonal
    if (i > 0 && j > 0)
    {
        neighbors.push_back(cells[(i - 1) + (j - 1) * rows]);

        /// X00
        /// 0#0
        /// 000
    }
    if (i < rows - 1 && j > 0)
    {
        neighbors.push_back(cells[(i + 1) + (j - 1) * rows]);

        /// 00X
        /// 0#0
        /// 000
    }
    if (i > 0 && j < columns - 1)
    {
        neighbors.push_back((cells[(i - 1) + (j + 1) * rows]));

        /// 000
        /// 0#0
        /// X00
    }
    if (i < rows - 1 && j < columns - 1)
    {
        neighbors.push_back(cells[(i + 1) + (j + 1) * rows]);

        /// 000
        /// 0#0
        /// 00X
    }
}
