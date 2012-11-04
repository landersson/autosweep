
#ifndef _MINE_FIELD_H
#define _MINE_FIELD_H

#include "Cell.h"
#include "Grid.h"

#include <vector>

class MineField : public Grid<Cell>
{
public:
    // constructor
    MineField(int rows = 0, int cols = 0, int num_mines = 0);

    void  resetCells();

    // initialize cell values (i.e, number of neighbours that are bombs)
    void setupValues();

    // randomly fill in <num> mines into board
    void randomlyFillMines(int num);

    bool loadFromFile(const std::string& filename);

    std::string toString() const;
    bool fromStrings(const std::vector<std::string>& rows);


private:
    void fail(const char* message) const;
};


#endif // _MINE_FIELD_H
