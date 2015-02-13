
#ifndef _NEIGHBOURS_H_
#define _NEIGHBOURS_H_

#include "Location.h"

#include <cassert>

class Neighbours;

class NeighboursIter
{
public:
    NeighboursIter(const Neighbours* neighbours, int pos) :
        _neighbours(neighbours),
        _pos(pos)
    { }

    bool operator!= (const NeighboursIter& other) const
    {
        return _pos != other._pos;
    }

    const Location& operator* () const;

    const NeighboursIter& operator++ ()
    {
        ++_pos;
        return *this;
    }

private:
    const Neighbours* _neighbours;
    int               _pos;
};

/*!
 * \class Neighbours
 * \brief Container specialized to contain the location 
 *        of a grid cells immediate neighbours  
 */
class Neighbours
{
public:
    Neighbours() : _num_locations(0) { }

    size_t size() const
    {
        return _num_locations;
    }

    Location& operator[](int index)
    {
        assert(index < _num_locations);
        return _locations[index];
    }

    const Location& operator[](int index) const
    {
        assert(index < _num_locations);
        return _locations[index];
    }

    // define begin() in order to use it as an iterator in range for loops
    NeighboursIter begin() const
    {
        return NeighboursIter(this, 0);
    }

    NeighboursIter end() const
    {
        return NeighboursIter(this, _num_locations);
    }

    void addLocation(const Location& location)
    {
        assert(_num_locations < 8);
        _locations[_num_locations] = location;
        _num_locations++;
    }

private:
    int      _num_locations;
    Location _locations[8];
};

#if 0
inline Location& NeighboursIter::operator*()
{
    return _neighbours->operator[](_pos);
}
#endif

inline const Location& NeighboursIter::operator*() const
{
    return _neighbours->operator[](_pos);
}


#endif // _NEIGHBOURS_H_
