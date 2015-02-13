
#ifndef _GRID_H
#define _GRID_H

#include "Location.h"
#include "Neighbours.h"

#include "choose.h"
#include <vector>


class GridSize
{
protected:
    int _rows;
    int _cols;
public:
    int locationToIndex(int i, int j) const
    {
        return i * _cols + j;
    }
    Location indexToLocation(int index) const
    {
        int row = index / _cols;
        int col = index - row * _cols;
        return Location(row, col);
    }
};

template<typename C> class Grid;

template<typename C, bool IsConst>
class GridIter
{
public:
    typedef std::forward_iterator_tag iterator_category;
    typedef C value_type;
    typedef std::ptrdiff_t difference_type;

    typedef typename choose<IsConst, const C&, C&>::type reference;
    typedef typename choose<IsConst, const C*, C*>::type pointer;
    typedef typename choose<IsConst, const Grid<C>*, Grid<C>*>::type gridptr;

    GridIter(gridptr grid, int row, int col) :
        _grid(grid),
        _location(row, col)
    { }

    bool operator!= (const GridIter& other) const
    {
        return !(_location == other._location);
    }

    reference operator* ();
    pointer operator-> ();

    const GridIter& operator++ ()
    {
        ++_location.col;
        if (_location.col >= _grid->getCols())
        {
            _location.col = 0;
            ++_location.row;
        }
        return *this;
    }

    const Location& getLocation() const
    {
        return _location;
    }
    
private:
    gridptr _grid;
    Location _location;
};



/*!
 * \class Grid
 * \brief Storage and access for a 2D grid of arbitrary elements
 */
template<typename C>
class Grid : public GridSize
{
public:
    typedef std::vector<Location>  LocationList;

    typedef GridIter<C, false> iterator;
    typedef GridIter<C, true>  const_iterator;


protected:
    std::vector<std::vector<C> > _cells;

public:

    Grid(int rows = 0, int cols = 0);
    ~Grid();

    void allocCells(int rows, int cols);
    void resetCells(const C & value);

    int getRows() const;
    int getCols() const;

    C& getCell(int i, int j);
    C& getCell(const Location& loc);

    const C& getCell(int i, int j) const;
    const C& getCell(const Location& loc) const;

    void setCell(const Location& loc, C c);
    bool empty() const;
    bool isValidCoordinate(int i, int j) const;

    // define begin() in order to use it as an iterator in range for loops
    const_iterator begin() const;
    const_iterator end() const;

    iterator begin();
    iterator end();

    Neighbours getNeighbours(int i, int j) const;

    Neighbours getNeighbours(const Location &loc) const
    {
        return getNeighbours(loc.row, loc.col);
    }
    

    std::string toString() const;
    bool fromStrings(const std::vector<std::string>& rows);

    void fail(const char* message) const;
};


//--- inline method declarations ----------------------------------------------


template<typename C, bool IsFalse>
inline typename GridIter<C, IsFalse>::reference GridIter<C, IsFalse>::operator*()
{
    return _grid->getCell(_location);
}

template<typename C, bool IsFalse>
inline typename GridIter<C, IsFalse>::pointer GridIter<C, IsFalse>::operator->()
{
    return &_grid->getCell(_location);
}



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

    _rows = rows;
    _cols = cols;

    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            C cell = C();
            _cells[i].push_back(cell);
        }
    }
}

template<typename C>
void Grid<C>::resetCells(const C & value)
{
    for (int i = 0; i < _rows; i++)
    {
        for (int j = 0; j < _cols; j++)
        {
            _cells[i][j] = value;
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
inline void Grid<C>::setCell(const Location& loc, C c)
{
    _cells[loc.row][loc.col] = c;
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
                continue;   // I'm not my neighbour

            if (!this->isValidCoordinate(i + di, j + dj))
                continue;   // outside grid

            res.addLocation(Location(i + di, j + dj));
        }
    }
    return res;
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
inline C& Grid<C>::getCell(int i, int j)
{
    if (!isValidCoordinate(i, j))
        throw("kalle");
    //        fail("Grid: tried to get cell from invalid coordinate");

    return _cells[i][j];
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

    // define begin() in order to use it as an iterator in range for loops
template<typename C>
inline typename Grid<C>::const_iterator Grid<C>::begin() const
{
    return const_iterator(this, 0, 0);
}

template<typename C>
inline typename Grid<C>::const_iterator Grid<C>::end() const
{
    return const_iterator(this, _rows, 0);
}

template<typename C>
inline typename Grid<C>::iterator Grid<C>::begin()
{
    return iterator(this, 0, 0);
}

template<typename C>
inline typename Grid<C>::iterator Grid<C>::end()
{
    return iterator(this, _rows, 0);
}




#endif // _GRID_H_
