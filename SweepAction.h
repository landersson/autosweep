
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

    SweepAction(Action action, int row, int col) : _action(action), _location(row, col) { }
    SweepAction(Action action, const Location& loc) : _action(action), _location(loc) { }

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


private:
    Action   _action;
    Location _location;
};

#endif // _SWEEP_ACTION_H
