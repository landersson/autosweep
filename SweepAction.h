
#ifndef _SWEEP_ACTION_H
#define _SWEEP_ACTION_H

#include "Location.h"

class SweepAction
{
public:
    enum Action
    {
        FLAG_CELL,
        CLICK_CELL
    };

    enum Flags
    {
        CERTAIN = 1
    };

#if 0
    SweepAction(Action action, int row, int col) :
        _action(action), 
        _location(row, col) 
    { }
#endif    
    SweepAction(Action action, const Location& loc, unsigned flags = 0) : 
        _action(action), 
        _location(loc),
        _flags(flags)
    { }

    Action getAction() const
    {
        return _action;
    }

    int getRow() const
    {
        return _location.row;
    }
    int getCol() const
    {
        return _location.col;
    }

    const Location& getLocation() const
    {
        return _location;
    }

    bool isCertain() const
    {
        return _flags & CERTAIN;
    }

private:
    Action   _action;
    Location _location;
    unsigned _flags;
};

#endif // _SWEEP_ACTION_H
