
#ifndef _CELL_H
#define _CELL_H

#include <string>

class Cell
{
public:
    Cell();

    enum
    {
        // for value
        MINE = -1          // not 0 - 9
    };
    enum
    {
        // for state
        FLAGGED  = 1,
        UNKNOWN  = 2,
        KNOWN    = 3
    };

    bool operator==(const Cell& c) const;
    bool operator!=(const Cell& c) const;

    bool isValidState(int state) const;
    bool isValidValue(int value) const;

    int  getValue() const;
    void setValue(int new_value);

    int  getState() const;
    void setState(int new_state);

    // returns true if cell state is known and
    // and has at least one mine next to it
    bool isNextToBombs() const;

    // format cell state/value as a textal string
    std::string toString() const;

    // parse Cell state/value from textual string
    bool fromString(const std::string& str);
private:
    int _value;                  //  value of the cell, can be 0-8, or -1=mine
    int _state;                  //  state value, can be MARKED, KNOWN or UNKNOWN
};


//--- inline method declarations ----------------------------------------------

inline int Cell::getValue() const
{
    return _value;
}

inline int Cell::getState() const
{
    return _state;
}

inline bool Cell::isNextToBombs() const
{
    if (_state == KNOWN && _value > 0)
        return true;
    else
        return false;
}

#endif // _CELL_H
