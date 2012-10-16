
#ifndef _LOCATION_H
#define _LOCATION_H

struct Location
{
    Location(int a_row = 0, int a_col = 0) : row(a_row), col(a_col) { }

    bool valid() const;

    int row;
    int col;
};

inline bool Location::valid() const
{
    return row >= 0 && col >= 0;
}

inline bool operator ==(const Location& l1, const Location& l2)
{
    return l1.row == l2.row && l1.col == l2.col;
}

#endif
