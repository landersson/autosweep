
#ifndef _MINE_FIELD_H
#define _MINE_FIELD_H

#include "Cell.h"
#include "Grid.h"
#include "Location.h"

#include <vector>

#define GRID

class MineField : public Grid<Cell>
{
public:

    MineField(int rows = 0, int cols = 0, int num_mines = 0);

    void  resetCells();

    void setupValues();

    // randomly fill in <num> mines into board
    void randomlyFillMines(int num);

    std::string toString() const;
    bool fromStrings(const std::vector<std::string>& rows);

    void fail(const char* message) const;
};


#endif // _MINE_FIELD_H
