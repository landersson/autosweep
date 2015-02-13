
#ifndef MINE_FIELD_H_
#define MINE_FIELD_H_

#include "Cell.h"
#include "Grid.h"

#include <vector>

/*!
 * \class MineField
 * \brief Represents a MineSweeper MineField and provides methods for initialization
 */
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

    int getNumCellsWithState(int state) const;

    bool isCorner(const Location &loc) const;
    bool isEdge(const Location &loc) const;

private:
    void fail(const char* message) const;
};


inline bool MineField::isCorner(const Location & loc) const
{
    return
        (loc.col == 0 && loc.row == 0) ||
        (loc.col == 0 && loc.row == (getRows() - 1)) ||
        (loc.row == 0 && loc.col == (getCols() - 1)) ||
        (loc.row == (getRows() - 1) && loc.col == (getCols() - 1));
}

inline bool MineField::isEdge(const Location & loc) const
{
    return
        (loc.col == 0 || 
         loc.row == 0 ||
         loc.row == (getRows() - 1) ||
         loc.col == (getCols() - 1));
}


#endif // MINE_FIELD_H_
