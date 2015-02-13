
#include "Cell.h"

#include <sstream>
#include <stdexcept>

Cell::Cell(int value, int state) : _value(value), _state(state) { }

bool Cell::operator ==(const Cell& c) const
{
    return _state == c._state && _value == c._value;
}

bool Cell::operator !=(const Cell& c) const
{
    return !(*this == c);
}

bool Cell::isValidState(int state) const
{
    return state == FLAGGED || state == KNOWN || state == UNKNOWN;
}

bool Cell::isValidValue(int value) const
{
    return (value >= 0 && value <= 8) || value == MINE;
}


void Cell::setState(int new_state)
{
    if (this->isValidState(new_state))
    {
        this->_state = new_state;
    }
    else
        throw std::invalid_argument("invalid state: "); // + str(new_state));
}

void Cell::setValue(int new_value)
{
    if (this->isValidValue(new_value))
    {
        this->_value = new_value;
    }
    else
        throw std::invalid_argument("invalid value");
}

std::string Cell::toString() const
{
    char value_char;
    char state_char;

    if (_value < 0)
        value_char = 'X';
    else if (_value == 0)
        value_char = '_';
    else
        value_char = '0' + _value;

    if (_state == UNKNOWN)
        state_char = '?';
    else if (_state == KNOWN)
        state_char = '!';
    else
        state_char = 'F';

    std::string s = "|";
    s += value_char;
    s += state_char;
    s += '|';

    return s;
}


bool Cell::fromString(const std::string& str)
{
    if (str.length() != 4) return false;

    if (str[0] != '|' || str[3] != '|') return false;

    if (str[1] == 'X')
        _value = -1;
    else if (str[1] == '_')
        _value = 0;
    else if (str[1] >= '1' && str[1] <= '9')
        _value = str[1] - '0';
    else
        return false;

    if (str[2] == 'F')
        _state = Cell::FLAGGED;
    else if (str[2] == '!')
        _state = Cell::KNOWN;
    else if (str[2] == '?')
        _state = Cell::UNKNOWN;
    else
        return false;

    return true;
}

