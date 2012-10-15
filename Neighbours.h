// ----------------------------------------------------------------------------
//
//! @brief       An utility class for generating indices based on a vector of
//!              integers or a continous range between 0 and an end value.
//! @note        Could be extended and generalized in a number of useful ways
//!              but no time for that right now.
//! @author      Lars Andersson
//! @date        2012
//! @copyright   National ICT Australia (NICTA). All rights reserved.
//
// ----------------------------------------------------------------------------

#ifndef _NEIGHBOURS_H_
#define _NEIGHBOURS_H_

#include "Location.h"

#include <cassert>

class Neighbours;

class NeighboursIter
{
public:
    NeighboursIter(const Neighbours* index_gen, int pos) :
        _index_gen(index_gen),
        _pos(pos)
    { }

    bool operator!= (const NeighboursIter& other) const
    {
        return _pos != other._pos;
    }

    //    Location& operator* ();
    const Location& operator* () const;

    const NeighboursIter& operator++ ()
    {
        ++_pos;
        return *this;
    }

private:
    const Neighbours* _index_gen;
    int               _pos;
};

class Neighbours
{
public:
    //! @brief Create an index generator returning a continous range
    //! of indices between 0 and continous_end - 1.

    Neighbours() : _num_locations(0) { }

    //! @brief Create an index generator returning indices from given
    //! vector of ints

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

    // in order to use it as an iterator in range for loops
    NeighboursIter begin() const
    {
        return NeighboursIter(this, 0);
    }

    NeighboursIter end() const
    {
        return NeighboursIter(this, _num_locations);
    }

    void push_back(const Location& location)
    {
        addLocation(location);
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
    return _index_gen->operator[](_pos);
}
#endif

inline const Location& NeighboursIter::operator*() const
{
    return _index_gen->operator[](_pos);
}


#endif // _INDEXGENERATOR_H_
