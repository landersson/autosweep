
#ifndef _GRID_H
#define _GRID_H

#include "Location.h"
#include "Neighbours.h"

#include <vector>

template<typename C>
class Grid
{
public:
    typedef std::vector<Location>  LocationList;

protected:
    int _rows, _cols;                            //  size of the board
    std::vector<std::vector<C> > _cells;

public:

    Grid(int rows = 0, int cols = 0);
    ~Grid();

    void  allocCells(int rows, int cols);
    void  resetCells();

    int   getRows() const;
    int   getCols() const;

    C& getCell(int i, int j);
    C& getCell(const Location& loc);

    const C& getCell(int i, int j) const;
    const C& getCell(const Location& loc) const;

    bool  empty() const;
    bool  isValidCoordinate(int i, int j) const;

    Neighbours getNeighbours(int i, int j) const;

    std::string toString() const;
    bool fromStrings(const std::vector<std::string>& rows);

    void fail(const char* message) const;
};


//--- inline method declarations ----------------------------------------------

template<typename C>
Grid<C>::Grid(int rows, int cols)
{
    allocCells(rows, cols);
}

template<typename C>
Grid<C>::~Grid()
{
}

template<typename C>
void Grid<C>::allocCells(int rows, int cols)
{
    _cells.clear();
    _cells.resize(rows);

    this->_rows = rows;
    this->_cols = cols;

    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            C cell;
            _cells[i].push_back(cell);
        }
    }
}

template<typename C>
inline bool Grid<C>::empty() const
{
    return _rows == 0 && _cols == 0;
}

template<typename C>
inline bool Grid<C>::isValidCoordinate(int i, int j) const
{
    return  i >= 0 && i < _rows && j >= 0 && j < _cols;
}

template<typename C>
inline int Grid<C>::getCols() const
{
    return _cols;
}

template<typename C>
inline int Grid<C>::getRows() const
{
    return _rows;
}

template<typename C>
inline C& Grid<C>::getCell(int i, int j)
{
    if (!isValidCoordinate(i, j))
        throw("kalle");
    //        fail("Grid: tried to get cell from invalid coordinate");

    return _cells[i][j];
}

template<typename C>
inline const C& Grid<C>::getCell(int i, int j) const
{
    if (!isValidCoordinate(i, j))
        throw("kalle");
    //        fail("Grid: tried to get cell from invalid coordinate");

    return _cells[i][j];
}

template<typename C>
Neighbours Grid<C>::getNeighbours(int i, int j) const
{
    Neighbours res;

    for (int di = -1; di <= 1; di++)
    {
        for (int dj = -1; dj <= 1; dj++)
        {
            if (di == 0 && dj == 0)
                continue;   //  I'm not my neighbour

            if (!this->isValidCoordinate(i + di, j + dj))
                continue;   //  outside board

            res.push_back(Location(i + di, j + dj));
        }
    }
    return res;
}

template<typename C>
inline C& Grid<C>::getCell(const Location& location)
{
    return this->getCell(location.row, location.col);
}

template<typename C>
inline const C& Grid<C>::getCell(const Location& location) const
{
    return this->getCell(location.row, location.col);
}


#endif // _GRID_H_
