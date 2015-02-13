
#ifndef _LOCATION_H
#define _LOCATION_H

struct Location
{
    Location() : row(0), col(0) { }
    Location(int a_row, int a_col) : row(a_row), col(a_col) { }

    bool valid() const;

//    bool operator== (const Location& other) const;
    
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

inline bool operator <(const Location& l1, const Location& l2)
{
    int x1 = l1.row * 10000 + l1.col;
    int x2 = l2.row * 10000 + l2.col;

    return x1 < x2;
}


#endif
